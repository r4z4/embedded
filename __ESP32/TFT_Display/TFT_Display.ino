#include<sMQTTBroker.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "Env.h"

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// Note that yield() or delay(0) must be called in long duration for/while
// loops to stop the ESP8266 watchdog triggering.

const char* MQTT_CLIENT_USER = "admin"; 
const char* MQTT_CLIENT_PASSWORD = "adminpwd"; 

class MyBroker:public sMQTTBroker
{
public:
  bool onEvent(sMQTTEvent *event) override
  {
    switch(event->Type())
    {
      case NewClient_sMQTTEventType:
        {
          sMQTTNewClientEvent *e=(sMQTTNewClientEvent*)event;
          // Check username and password used for new connection
          if ((e->Login() != MQTT_CLIENT_USER) || (e->Password() != MQTT_CLIENT_PASSWORD)) {
              Serial.println("Invalid username or password");  
              return false;
            }
        };
        incrementCount();
        break;
      case LostConnect_sMQTTEventType:
        WiFi.reconnect();
        break;
      case UnSubscribe_sMQTTEventType:
      case Subscribe_sMQTTEventType:
        {
          sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;
        }
        break;
    }
    return true;
  }
};

MyBroker broker;

void setup()
{
  // Setup TFT
  tft.init();
  tft.setRotation(1);

  Serial.begin(115200);
  // Set up Wi-Fi in Access Point mode
  const char *ssid = "SSID";
  const char *password = "Password";
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
  }
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
   
  const unsigned short mqttPort=1883;
  broker.init(mqttPort);
  tft.println("WiFi & MQTT Initialized and Ready");
};

void loop()
{
    broker.update();
    // Code
};

unsigned long connCt = 0;

void incrementCount() {
  connCt++;
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.print("Connections = "); tft.println(connCt); 
  // for (int i = 0; i < 1000; i++) {
  //   yield(); tft.drawNumber(i, 0, 0, 7);
  // }
}