// Sending Data via WiFi

#include<WiFi.h>

const char* ssid = "";
const char* password = "";

void setup() {
  Serial.being(115200);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED) {
    
  }
}