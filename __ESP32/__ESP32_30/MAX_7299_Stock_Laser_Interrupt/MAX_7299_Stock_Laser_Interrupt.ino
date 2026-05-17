/*
 * ESP32 C3 Stock Ticker for MAX7219 8x32 Matrix
 * 
 * Hardware:
 * - ESP32 C3 SuperMini
 * - MAX7219 8x32 LED Matrix (4x 8x8 modules)
 * 
 * Pinout ESP32C3:
 * DIN  -> GPIO 6 (MOSI)
 * CS   -> GPIO 7 (CS)
 * CLK  -> GPIO 4 (SCK)
 * VCC  -> 5V
 * GND  -> GND

 * Pinout ESP32 (38):
 * DIN  -> GPIO 23 (MOSI)
 * CS   -> GPIO 2 (CS)
 * CLK  -> GPIO 18 (SCK)
 * VCC  -> 5V
 * GND  -> GND

* Pinout ESP32 (30):
 * DIN  -> GPIO 23 (MOSI)
 * CS   -> GPIO 5 (CS)
 * CLK  -> GPIO 18 (SCK)
 * VCC  -> 3.3V
 * GND  -> GND
 * 
 * Required Libraries:
 * - MD_Parola
 * - MD_MAX72XX
 * - ArduinoJson
 */
 
#include "Env.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

RTC_DATA_ATTR int bootCount = 0;

// --- Configuration ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* apiKey = FINNHUB_API_KEY; 

// List of stock symbols to track
const char* stocks[] = {"ALB","AMD","BSX","CSIQ","EMBJ","GRAB","HIMS","IONQ","LOGI","QBTS"};
const int numStocks = sizeof(stocks) / sizeof(stocks[0]);

// Hardware SPI connection
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    5
#define TOUCH_RECEIVER_PIN 4
gpio_num_t gpio_touch_pin = (gpio_num_t)TOUCH_RECEIVER_PIN;

// MD_Parola instance
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Global variables
String tickerText = "Fetching Stocks...";
int scrollCount = 0;
const int maxScrolls = 3;

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(TOUCH_RECEIVER_PIN, INPUT);
  ++bootCount;
  Serial.println("Boot Number: " + String(bootCount));
  print_wakeup_reason();
  esp_sleep_enable_ext0_wakeup(gpio_touch_pin, 1); // Use pull-down resistor to avoid accidental wakeups
  // Initialize Display
  P.begin();
  P.setIntensity(5); // Brightness 0-15
  P.displayScroll(tickerText.c_str(), PA_LEFT, PA_SCROLL_LEFT, 100);

  // Connect to WiFi
  connectToWiFi();
}

void loop() {
  // Update display animation
  if (P.displayAnimate()) {
    scrollCount++;
    
    // After scrolling 3 times, fetch new data
    if (scrollCount >= maxScrolls) {
      scrollCount = 0;
      // updateTickerText();
      // P.displayReset();
      // Go to sleep
      Serial.println("Going to Sleep");
      WiFi.disconnect(true);
      WiFi.mode(WIFI_OFF);
      Serial.flush();
      esp_deep_sleep_start(); // If looping we are awake
    } else {
      P.displayReset();
    }
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected!");
  updateTickerText(); // Initial fetch
}

void updateTickerText() {
  String newTicker = "";
  HTTPClient http;

  for (int i = 0; i < numStocks; i++) {
    String url = "https://finnhub.io/api/v1/quote?symbol=" + String(stocks[i]) + "&token=" + String(apiKey);
    
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      DynamicJsonDocument doc(1024);
      deserializeJson(doc, payload);
      
      float price = doc["c"]; // current price in Finnhub response
      float percentChange = doc["dp"]; // percent change
      newTicker += String(stocks[i]) + ": $" + String(price, 2) + " (" + String(percentChange, 2) + "%)   ";
    } else {
      Serial.printf("Error fetching %s: %d\n", stocks[i], httpCode);
      newTicker += String(stocks[i]) + ": Error   ";
    }
    http.end();
  }

  if (newTicker != "") {
    tickerText = newTicker;
    P.displayClear();
    P.displayScroll(tickerText.c_str(), PA_LEFT, PA_SCROLL_LEFT, 100);
    Serial.println("Ticker updated: " + tickerText);
  }
}
