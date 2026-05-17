#include <WiFi.h>
#include "esp_now.h"

// bool begin(const uint8_t *pmk = NULL);

// Capactive touch sensor. But we are ignoring 4 for now
const int BUTTON1_PIN = 36;
const int BUTTON2_PIN = 39;
const int BUTTON3_PIN = 34;

typedef struct ButtonData {
  bool button1;
  bool button2;
  bool button3;
} ButtonData;

ButtonData data;

// Receiver MAC = Station MAC: 68:25:DD:44:CE:D4
uint8_t receiverMAC[] = {0x68, 0x25, 0xDD, 0x44, 0xCE, 0xD4}; // Hex

void setup() {
  Serial.begin(115200);
  pinMode(BUTTON1_PIN, INPUT);
  pinMode(BUTTON2_PIN, INPUT);
  pinMode(BUTTON3_PIN, INPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

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
}

void loop() {
  data.button1 = digitalRead(BUTTON1_PIN) == HIGH;
  data.button2 = digitalRead(BUTTON2_PIN) == HIGH;
  data.button3 = digitalRead(BUTTON3_PIN) == HIGH;

  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

  if (digitalRead(BUTTON1_PIN) == HIGH) {
    Serial.println("1 Pressed");
    // data.button1 = true;
  }

  if (digitalRead(BUTTON2_PIN) == HIGH) {
    Serial.println("2 Pressed");
    // data.button2 = true;
  }

  if (digitalRead(BUTTON3_PIN) == HIGH) {
    Serial.println("3 Pressed");
    // data.button3 = true;
  }

  // esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));


  delay(100);
}