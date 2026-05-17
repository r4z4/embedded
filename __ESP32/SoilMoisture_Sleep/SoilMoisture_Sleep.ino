#include "Env.h"
#include <Arduino.h>
#include <WiFi.h>

#define WIFI_SSID WIFI_SSID
#define WIFI_PASSWORD WIFI_PASS

int TIME_TO_SLEEP = 300;
unsigned long long uS_TO_S_FACTOR = 1000000;
RTC_DATA_ATTR int bootCount = 0;
// Define the sensor pin
const int sensorPin = 25;
int moistureLevel;

void setup() {
    // Do Stuff
    Serial.begin(115200);
    ++bootCount;
    Serial.println("Boot Number: " + String(bootCount));
    wifi_connect();
    moistureLevel = analogRead(sensorPin);
    // Send via MQTT
    Serial.println("I'm tired. Off to sleep ...");
    // Wake up source
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    // Prepare for sleep
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.flush();
    // Enable sleep
    esp_deep_sleep_start();
}

void loop() {}