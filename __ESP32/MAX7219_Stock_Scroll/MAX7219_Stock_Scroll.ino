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

 * Pinout ESP32:
 * DIN  -> GPIO 23 (MOSI)
 * CS   -> GPIO 2 (CS)
 * CLK  -> GPIO 18 (SCK)
 * VCC  -> 5V
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

// --- Configuration ---
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;
const char* apiKey = FINNHUB_API_KEY; 

// List of stock symbols to track
const char* stocks[] = {"AMD", "ALB", "GRAB", "HIMS", "QBTS"};
const int numStocks = sizeof(stocks) / sizeof(stocks[0]);

// Hardware SPI connection
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CLK_PIN   18
#define DATA_PIN  23
#define CS_PIN    2

// MD_Parola instance
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Global variables
String tickerText = "Fetching Stocks...";
int scrollCount = 0;
const int maxScrolls = 3;

void setup() {
  Serial.begin(115200);
  
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
      updateTickerText();
      P.displayReset();
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
      
      float price = doc["c"]; // 'c' is the current price in Finnhub response
      newTicker += String(stocks[i]) + ": $" + String(price, 2) + "   ";
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
