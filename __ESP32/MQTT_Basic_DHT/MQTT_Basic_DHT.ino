// #include <SPI.h>
#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <dht_nonblocking.h>

#define MAX 100 
#define DHT_SENSOR_TYPE DHT_TYPE_11
#define MAX_MQTT_CONN_ATTEMPTS 5

// Use this to track retries
int num_mqtt_connection_attempts = 0;

static const int DHT_SENSOR_PIN = 26;
DHT_nonblocking dht_sensor( DHT_SENSOR_PIN, DHT_SENSOR_TYPE );

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

// Add your MQTT Broker IP address, example:
// const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_DHT";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

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
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  // if (!bme.begin(0x76)) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1);
  // }
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  // pinMode(ledPin, OUTPUT);
  if (!client.connected()) {
    try_reconnect();
  }
}

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

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.print(topic);
  Serial.print(". Message: ");
  String messageTemp;
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }
  Serial.println();
  // If a message is received on the topic esp32/output, you check if the message is either "on" or "off". 
  // Changes the output state according to the message
  if (String(topic) == "esp32/output") {
    Serial.print("Changing output to ");
    if(messageTemp == "on"){
      Serial.println("on");
      // digitalWrite(ledPin, HIGH);
    }
    else if(messageTemp == "off"){
      Serial.println("off");
      // digitalWrite(ledPin, LOW);
    }
  }
}

char* toCharArray(std::string str) {
  return &str[0];
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

void loop() {
  float temperature;
  float humidity;
  
  /* Measure temperature and humidity.  If the functions returns
     true, then a measurement is available. */
  if( measure_environment( &temperature, &humidity ) == true )
  {
    // Serial.print( "T = " );
    // Serial.print( temperature, 1 );
    // Serial.print( " deg. C, H = " );
    // // std::string temp_str = sprintf("Temp: ", temperature);
    // client.publish("esp32/temp", "Temp: ");
    // char temp_buf[MAX]; 
    // snprintf (temp_buf, sizeof(temp_buf), "%f", temperature);
    // client.publish("esp32/temp", temp_buf);
    // Serial.print( humidity, 1 );
    // Serial.println( "%" );
    // // std::string hum_str = sprintf("Humidity: ", humidity);
    // client.publish("esp32/humidity", "Humidity: ");
    // char hum_buf[MAX]; 
    // snprintf (hum_buf, sizeof(hum_buf), "%f", humidity);
    // client.publish("esp32/humidity", hum_buf);
    
    char full_message[MAX];
    snprintf(full_message, sizeof(full_message), "Temp: %.1f; Hum: %.1f", temperature, humidity);
    Serial.print(full_message);
    // Publish the string
    client.publish("esp32/sensor_data", full_message);
  }
}