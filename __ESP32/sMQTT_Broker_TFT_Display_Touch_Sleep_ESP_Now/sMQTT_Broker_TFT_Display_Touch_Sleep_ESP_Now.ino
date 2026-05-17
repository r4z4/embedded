#include<sMQTTBroker.h>
#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "esp_now.h"
#include "Env.h"

#define MAX 120
#define TOUCH_PIN_OFF 32 // T9
#define TOUCH_PIN_ON 33 // T8
#define THRESHOLD 20
#define LED_PIN 22
// #define VCC_CTL 27

touch_pad_t touchPin;
char full_message[MAX];

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data received;

TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// Note that yield() or delay(0) must be called in long duration for/while
// loops to stop the ESP8266 watchdog triggering.

const char* MQTT_CLIENT_USER = MQTT_USER; 
const char* MQTT_CLIENT_PASSWORD = MQTT_PASS;

RTC_DATA_ATTR int bootCount = 0;
int capacitiveTouch = 0;
std::string newClientId;

class MyBroker:public sMQTTBroker
{
public:
  unsigned long connCt = 0;
  unsigned long subCt = 0;

  void printScreen() {
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
    tft.setTextFont(2);
    tft.print("Connections = "); tft.println(connCt); 
    tft.println(""); 
    tft.println(""); 
    tft.setTextColor(TFT_WHITE,TFT_BLUE);  tft.setTextSize(1);
    tft.setTextFont(2);
    tft.print("Subscriptions = "); tft.println(subCt); 
    // for (int i = 0; i < 1000; i++) {
    //   yield(); tft.drawNumber(i, 0, 0, 7);
    // }
  }

  void alterSubCount(int type) {
    if (type == 0) {
      subCt--;
    } else if (type == 1) {
      subCt++;
    }
    printScreen();
  }

  void alterConnCount(int type) {
    if (type == 0) {
      connCt--;
    } else if (type == 1) {
      connCt++;
    }
    printScreen();
  }

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
          sMQTTClient *newClient = e->Client();
          newClientId = newClient->getClientId();
          Serial.print("New Client Id = ");
          Serial.println(newClientId.c_str()); // Expects a const char*
        };
        Serial.println("New Client");
        alterConnCount(1); // Inc
        break;
      case Public_sMQTTEventType:
        Serial.println("Public Event");
        break;
      case RemoveClient_sMQTTEventType:
        Serial.println("Client Left");
        alterConnCount(0); // Dec
        break;
      case LostConnect_sMQTTEventType:
        WiFi.reconnect();
        break;
      case UnSubscribe_sMQTTEventType:
        {
          sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;
        }
        Serial.println("Unsubscribe");
        alterSubCount(0); // Dec
        break;
      case Subscribe_sMQTTEventType:
        {
          sMQTTSubUnSubClientEvent *e=(sMQTTSubUnSubClientEvent*)event;
        }
        Serial.println("Subscribe");
        alterSubCount(1); // Inc
        break;
    }
    return true;
  }
};

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

void print_wakeup_touchpad() {
  int touchPin = esp_sleep_get_touchpad_wakeup_status();

#if CONFIG_IDF_TARGET_ESP32
  switch (touchPin) {
    case 0:  Serial.println("Touch detected on GPIO 4"); break;
    case 1:  Serial.println("Touch detected on GPIO 0"); break;
    case 2:  Serial.println("Touch detected on GPIO 2"); break;
    case 3:  Serial.println("Touch detected on GPIO 15"); break;
    case 4:  Serial.println("Touch detected on GPIO 13"); break;
    case 5:  Serial.println("Touch detected on GPIO 12"); break;
    case 6:  Serial.println("Touch detected on GPIO 14"); break;
    case 7:  Serial.println("Touch detected on GPIO 27"); break;
    case 8:  Serial.println("Touch detected on GPIO 33"); break;
    case 9:  Serial.println("Touch detected on GPIO 32"); break;
    default: Serial.println("Wakeup not by touchpad"); break;
  }
#else
  if (touchPin < TOUCH_PAD_MAX) {
    Serial.printf("Touch detected on GPIO %d\n", touchPin);
  } else {
    Serial.println("Wakeup not by touchpad");
  }
#endif
}

MyBroker broker;

void buildMqttString(char* topic, char* entry) {
  // snprintf(full_message, sizeof(full_message), "topic: %s, entry: %s", topic, entry);
  Serial.println("Publishing ...");
  broker.publish(topic, entry, 0, false);
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(Data)) {
    Serial.println("Received Message");
    memcpy(&received, incomingData, sizeof(received));
    buildMqttString(received.topic, received.entry);
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  ++bootCount;
  Serial.println("Boot Number: " + String(bootCount));
  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();
  touchSleepWakeUpEnable(TOUCH_PIN_ON, THRESHOLD);
  Serial.println("Screen turning off now");
  digitalWrite(4, HIGH);
  // Setup TFT
  tft.init();
  tft.setCursor(0, 0);
  tft.setRotation(1);
  // Set up Wi-Fi in Access Point mode
  const char *ssid = WIFI_SSID;
  const char *password = WIFI_PASS;
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { // Wait for the Wi-Fi to connect
    delay(1000);
  }
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());
   
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW Init Failed");
    return;
  }
  esp_now_register_recv_cb(onReceive);

  const unsigned short mqttPort=1883;
  broker.init(mqttPort);
  tft.fillScreen(TFT_BLACK);
  tft.println("WiFi & MQTT Initialized and Ready");
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
};

void loop()
{
  broker.update();
  capacitiveTouch = touchRead(TOUCH_PIN_OFF); // touchRead when using actual pin
  // capacitiveState = digitalRead(TOUCH_PIN); // digital when TTP223
  // Serial.print("Touch reading: ");
  // Serial.println(capacitiveTouch);
  if( capacitiveTouch > 5 && capacitiveTouch < THRESHOLD ){
    Serial.println("Sensing a Touch");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(TFT_BL, LOW);
    tft.writecommand(ST7735_DISPOFF);// Switch off the display
    tft.writecommand(ST7735_SLPIN);// Sleep the display driver
    Serial.flush();
    Serial.println("Going to sleep.");
    esp_deep_sleep_start(); // If looping we are awake
  }
  // Code
};