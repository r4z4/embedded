#include <Wire.h>
#include <BH1750.h>
#include "esp_now.h"

#define MAX 100 
#define LED_PIN 6
#define TOUCH_PIN 5 // RTC GPIO

BH1750 lightMeter;

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data data;

char* topic = "ESP32_GY_302";

// Receiver MAC = Station MAC: 00:4B:12:2E:86:94 (ESP32 MQTT Broker)
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x2E, 0x86, 0x94}; // Hex

int sensorRunning = 0;

void IRAM_ATTR toggleSensor()
{
  sensorRunning = !sensorRunning;
  if (sensorRunning) {
    digitalWrite(LED_PIN, HIGH);
    // Loop and read
    for (;;) {
      float lux = lightMeter.readLightLevel();
      Serial.print("Light: ");
      Serial.print(lux);
      Serial.println(" lx");
      
      // char full_message[MAX];
      // snprintf(full_message, sizeof(full_message), "Light (lx): %f", lux);
      // data.topic = topic;
      // data.entry = full_message;
      // esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
      // delay(2000);
    }
  } else {
    digitalWrite(LED_PIN, LOW);
    // Go to sleep
    Serial.flush();
  }

}

void setup(){
  Serial.begin(115200);
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // if (esp_now_init() != ESP_OK) {
  //   Serial.println("ESP NOW Init Failed");
  //   return;
  // }
  // esp_now_peer_info_t peerInfo = {};
  // memcpy(peerInfo.peer_addr, receiverMAC, 6);
  // peerInfo.channel = 0;
  // peerInfo.encrypt = false;
  // if (esp_now_add_peer(&peerInfo) != ESP_OK) {
  //   Serial.println("Failed to add peer");
  //   return;
  // }

  lightMeter.begin();
  lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE);
  Serial.println(F("Light Meter Configured. Attaching Interrupt."));
  attachInterrupt(TOUCH_PIN, toggleSensor, RISING);
}

void loop() {}