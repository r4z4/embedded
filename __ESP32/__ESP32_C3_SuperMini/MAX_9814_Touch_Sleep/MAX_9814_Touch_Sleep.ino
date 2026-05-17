#include <Wire.h>
#include <BH1750.h>
#include "esp_now.h"

#define MAX 100 
#define LED_PIN 6
#define TOUCH_PIN 5 // RTC GPIO
#define SOUND_PIN 21

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data data;

const char* topic = "ESP32_MAX_9814";

// Receiver MAC = Station MAC: 00:4B:12:2E:86:94 (ESP32 MQTT Broker)
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x2E, 0x86, 0x94}; // Hex

void setup(){
  Serial.begin(115200);
  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  pinMode(SOUND_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  gpio_wakeup_enable(GPIO_NUM_5, GPIO_INTR_LOW_LEVEL);
  esp_sleep_enable_gpio_wakeup();
}

unsigned long startTime = millis(); // Store the starting time
unsigned long currentTime;
unsigned long oneMinute = 60000; // One minute in milliseconds

int count = 0;

void loop() {
  if (count > 300) {
    // startTime = currentTime; // Reset the timer
    Serial.println("Shutting Down");
    digitalWrite(LED_PIN, LOW);
    // Go to sleep
    Serial.flush();
    esp_deep_sleep_start();
  } else {
    count++;
    int voice = analogRead(SOUND_PIN);
    Serial.print(count);
    Serial.print(" : ");
    Serial.println(voice);
    delay(100);
  }
}