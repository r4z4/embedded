#include <WiFi.h>
#include <HTTPClient.h>
#include "Env.h"
#include <ArduinoJson.h>

const char* NCAAF = "http://site.api.espn.com/apis/site/v2/sports/football/college-football/scoreboard";
const char* NFL = "https://site.api.espn.com/apis/site/v2/sports/football/nfl/scoreboard";

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to WiFi");

  HTTPClient http;
  http.begin(NFL);
  int httpResponseCode = http.GET();

  if (httpResponseCode > 0) {
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    String payload = http.getString();
    Serial.printf("HTTP Response code: %d\n", httpResponseCode);
    String payload = http.getString();
    // NFL etc.  TooDeep error
    const size_t capacity = JSON_OBJECT_SIZE(200) + JSON_ARRAY_SIZE(200) + 60;
    DynamicJsonDocument doc(capacity);
    // Serial.println(payload);
    DeserializationError error = deserializeJson(doc, payload);

    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    // Access the data
    String first = doc["leagues"][0]["name"]; 
    // int sensorValue = doc["value"];       // Example: Accessing an integer value

    Serial.print("First: ");
    Serial.println(first);
    Serial.println(payload);
  } else {
    Serial.printf("Error code: %d\n", httpResponseCode);
  }
  http.end();
}

void loop() {
  // Your main application logic here
}

void toJson(payload) {
  const size_t capacity = JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(2) + 60; // Adjust capacity based on your JSON structure
  DynamicJsonDocument doc(capacity); // Or StaticJsonDocument if memory is fixed

  DeserializationError error = deserializeJson(doc, payload);

  if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
  }

  // Access the data
  String sensorType = doc["sensorType"]; // Example: Accessing a string value
  int sensorValue = doc["value"];       // Example: Accessing an integer value

  Serial.print("Sensor Type: ");
  Serial.println(sensorType);
  Serial.print("Sensor Value: ");
  Serial.println(sensorValue);
}
