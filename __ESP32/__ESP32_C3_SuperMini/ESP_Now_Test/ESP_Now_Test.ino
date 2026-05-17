#include "Env.h"
#include <WiFi.h>
#include <HTTPClient.h>


#define MAX 100 

// ESP32_NOW
#include "esp_now.h"

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data data;

char* topic = "ESP32_HC_SR04";

// Receiver MAC = Station MAC: 00:4B:12:2E:86:94 (ESP32 MQTT Broker)
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x2E, 0x86, 0x94}; // Hex


void setup_wifi() {
  WiFi.mode(WIFI_STA); // ESP Now requires WiFi header. Goes into STA mode.
  WiFi.disconnect();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW Init Failed");
    return;
  }
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  takeReading();
}

void takeReading() {

  char full_message[MAX];
  snprintf(full_message, sizeof(full_message), "In: %lu; Cm: %lu", 99, 77);
  // Publish the string
  // client.publish("esp32/sensor_data_hc_sr04", full_message);

  data.topic = topic;
  data.entry = full_message;
  Serial.println("Sending ESP NOW message");
  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
}

void loop() {}

