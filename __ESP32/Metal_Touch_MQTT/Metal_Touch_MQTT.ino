#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>

#define MAX_MQTT_CONN_ATTEMPTS 5

// Use this to track retries
int num_mqtt_connection_attempts = 0;

// MQTT
const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS; 

const char* mqtt_client_name = "ESP32_Rotary_Plus_IR_Receiver";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(const char* ssid, const char* password) {
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

// int digitalPin = 7;   // KY-036 digital interface
int analogPin = 36;   // KY-036 analog interface
// int ledPin = 13;      // Arduino LED pin
// int digitalVal;       // digital readings
int analogVal;        // analog readings

void setup()
{
  setup_wifi(WIFI_SSID, WIFI_PASS);
  client.setServer(mqtt_server, 1883);
  pinMode(analogPin, INPUT);
  // pinMode(digitalPin,INPUT); 
  // pinMode(ledPin,OUTPUT);      
  Serial.begin(9600);
  if (!client.connected()) {
    try_reconnect();
  }
}

void loop()
{
  // // Read the digital inteface
  // digitalVal = digitalRead(digitalPin); 
  
  // if(digitalVal == HIGH) 
  // {
  //   digitalWrite(ledPin, HIGH); // Turn ON Arduino's LED
  // }
  // else
  // {
  //   digitalWrite(ledPin, LOW);  // Turn OFF Arduino's LED
  // }

  // Read analog interface
  analogVal = analogRead(analogPin);
  Serial.println(analogVal);  // Print analog value to serial

  delay(100);
}