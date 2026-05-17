#include "Env.h"
#include<Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#define MAX 100
#define MAX_MQTT_CONN_ATTEMPTS 5

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

const char* mqtt_server = MQTT_Pixel_Server;
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_Gyro";
const char* mqtt_client_name = "ESP32_RGB";

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void  setup(){
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);
}
void loop(){
  if (!client.connected()) {
    reconnect();
  }
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,12,true);  
  AcX=Wire.read()<<8|Wire.read();    
  AcY=Wire.read()<<8|Wire.read();  
  AcZ=Wire.read()<<8|Wire.read();  
  GyX=Wire.read()<<8|Wire.read();  
  GyY=Wire.read()<<8|Wire.read();  
  GyZ=Wire.read()<<8|Wire.read();  
  
  Serial.print("Accelerometer: ");
  Serial.print("X = "); Serial.print(AcX);
  Serial.print(" | Y = "); Serial.print(AcY);
  Serial.print(" | Z = ");  Serial.println(AcZ); 
  
  Serial.print("Gyroscope: ");
  Serial.print("X  = "); Serial.print(GyX);
  Serial.print(" | Y = "); Serial.print(GyY);
  Serial.print(" | Z = "); Serial.println(GyZ);
  Serial.println(" ");

  char full_message[MAX];
  // %ld for signed long
  snprintf(full_message, sizeof(full_message), "AcX:%ld;AcY:%ld;AcZ:%ld||GyX:%ld;GyY:%ld;GyZ:%ld", AcX, AcY, AcZ, GyX, GyY, GyZ);
  client.publish("esp32/gyro", full_message);

  delay(333);
}