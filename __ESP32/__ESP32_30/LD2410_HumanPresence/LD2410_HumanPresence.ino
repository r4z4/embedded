#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ld2410.h>

#define MONITOR_SERIAL Serial
#define RADAR_SERIAL Serial1
#define RADAR_RX_PIN 16
#define RADAR_TX_PIN 17
#define LED_PIN 4

#define MAX_MQTT_CONN_ATTEMPTS 5
#define MAX 80

const char *ssid = WIFI_SSID; 
const char *password = WIFI_PASS; 
// MQTT
const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_LD2410";

// Use this to track retries
int num_mqtt_connection_attempts = 0;

int movement = 0; // Set to 1 if moving target
int distance = 0; // Distance in CM
int energy = 0; // Distance in CM

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

// RESET if need to try again
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

ld2410 radar;
 
uint32_t lastReading = 0;
 
void setup(void)
{
  MONITOR_SERIAL.begin(115200); // Feedback over Serial Monitor
  // radar.debug(MONITOR_SERIAL); Uncomment to show debug information from the library on the Serial Monitor. By default this does not show sensor reads as they are very frequent.
 
  RADAR_SERIAL.begin(256000, SERIAL_8N1, RADAR_RX_PIN, RADAR_TX_PIN); // UART for monitoring the radar
  delay(500);
  MONITOR_SERIAL.print(F("\nConnect LD2410 radar TX to GPIO:"));
  MONITOR_SERIAL.println(RADAR_RX_PIN);
  MONITOR_SERIAL.print(F("Connect LD2410 radar RX to GPIO:"));
  MONITOR_SERIAL.println(RADAR_TX_PIN);
  MONITOR_SERIAL.print(F("LD2410 radar sensor initialising: "));

  pinMode(LED_PIN, OUTPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    try_reconnect();
  }
 
  if (radar.begin(RADAR_SERIAL))
  {
    MONITOR_SERIAL.println(F("OK"));
    MONITOR_SERIAL.print(F("LD2410 firmware version: "));
    MONITOR_SERIAL.print(radar.firmware_major_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.print(radar.firmware_minor_version);
    MONITOR_SERIAL.print('.');
    MONITOR_SERIAL.println(radar.firmware_bugfix_version, HEX);
  }
  else
  {
    MONITOR_SERIAL.println(F("not connected"));
  }
}
 
void loop()
{
  radar.read();
  if (radar.isConnected() && millis() - lastReading > 1000) // Report every 1000ms
  {
    lastReading = millis();
    if (radar.presenceDetected())
      {
        if (radar.stationaryTargetDetected())
          {
            digitalWrite(LED_PIN, LOW);
            Serial.print(F("Stationary target: "));
            Serial.print(radar.stationaryTargetDistance());
            Serial.print(F("cm energy:"));
            Serial.print(radar.stationaryTargetEnergy());
            Serial.print(' ');

            movement = 2;
            distance = radar.stationaryTargetDistance();
            energy = radar.stationaryTargetEnergy();
          }
        if (radar.movingTargetDetected())
          {
            digitalWrite(LED_PIN, HIGH);
            Serial.print(F("Moving target: "));
            Serial.print(radar.movingTargetDistance());
            Serial.print(F("cm energy:"));
            Serial.print(radar.movingTargetEnergy());

            movement = 1;
            distance = radar.movingTargetDistance();
            energy = radar.movingTargetEnergy();
          }
        Serial.println();
      }
    else
      {
        digitalWrite(LED_PIN, LOW);
        Serial.println(F("No target"));
        movement = 0;
        distance = 0;
        energy = 0;
      }

    char full_message[MAX];
    // snprintf(full_message, sizeof(full_message), "Resistance: %lu", sensor);
    snprintf(full_message, sizeof(full_message), "Mv: %d; Dist: %d; Energy: %d", movement, distance, energy);
    client.publish("esp32/ld2410", full_message);
    delay(200);
  }
}