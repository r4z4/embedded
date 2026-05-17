// Sending Data via WiFi
#include "Env.h"
#include<WiFi.h>
#include<ESPAsyncWebServer.h> // From GitHub

const char* ssid = WIFI_SSID;
const char* password = WIFI_PASS;

AsyncWebServer server(80);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.println("Connecting...");
  }
  Serial.println("Connected to WiFi");
  
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/html", "<h1>Hello ESP32</h1>");
  });

  server.on("/", HTTP_POST, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "POST request received");
  });

  server.begin();
  Serial.println(WiFi.localIP()); // Get local IP of ESP32 so we can access it via network
}

void loop() {

}