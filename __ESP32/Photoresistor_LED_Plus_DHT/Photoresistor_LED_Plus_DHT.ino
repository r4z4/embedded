#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <dht_nonblocking.h>
#define DHT_SENSOR_TYPE DHT_TYPE_11

static const int DHT_SENSOR_PIN = 26;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

const int ledPin = 25;
const int sensorPin = 36; 
int sensor;
const int threshold = 200; // Threshold to turn LED on

#define MAX_MQTT_CONN_ATTEMPTS 5
#define MAX 5 

const char *ssid = WIFI_SSID; 
const char *password = WIFI_PASS; 
// MQTT
const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS; 

const char* mqtt_client_name = "ESP32_Photoresistor_DHT";

// Use this to track retries
int num_mqtt_connection_attempts = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void try_reconnect() {
  while (num_mqtt_connection_attempts < MAX_MQTT_CONN_ATTEMPTS) {
    num_mqtt_connection_attempts++;
    Serial.print("Attempting MQTT connection (Attempt #");
    Serial.print(num_mqtt_connection_attempts);
    Serial.print(")...");
    // Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      break;
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
      try_reconnect();
    }
  }
}

static bool measure_environment( float *temperature, float *humidity )
{
  static unsigned long measurement_timestamp = millis( );

  /* Measure once every four seconds. */
  if( millis( ) - measurement_timestamp > 3000ul )
  {
    if( dht_sensor.measure( temperature, humidity ) == true )
    {
      measurement_timestamp = millis( );
      return( true );
    }
  }

  return( false );
}

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT_PULLUP);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    try_reconnect();
  }
}

void loop() {
  float temperature;
  float humidity;

  if( measure_environment( &temperature, &humidity ) == true )
  {
    char temp_message[50];
    snprintf(temp_message, sizeof(temp_message), "Temp: %.1f; Hum: %.1f", temperature, humidity);
    // Publish the string
    Serial.print(temp_message);
    client.publish("esp32/sensor_data", temp_message);
  }

  sensor = analogRead(sensorPin);
  Serial.println(sensor);
  if(sensor < threshold) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
  char full_message[MAX];
  snprintf(full_message, sizeof(full_message), "%lu", sensor);
  client.publish("esp32/photoresistor", full_message);
  delay(200);
}
