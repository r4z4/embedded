#include "driver/rtc_io.h"
#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "time.h"

#define USE_EXT0_WAKEUP 1
#define WAKEUP_GPIO GPIO_NUM_4
#define WAKEUP_GPIO_BITMASK (1ULL << GPIO_NUM_4)

// Red and Green are reversed on this modiue? Super annoying
#define RED_LED_PIN 0
#define BLUE_LED_PIN 3
#define GREEN_LED_PIN 7

const char *ssid = WIFI_SSID; // Enter your WiFi name
const char *password = WIFI_PASS;  // Enter WiFi password
const unsigned long wifi_timeout = 10000; // Time to wait for WiFi connect

// Signal
String apiKey = SIGNAL_API_KEY;
String phone_number = SIGNAL_PHONE_NUMBER;

char signalMessage[50];
String url; // URL String used to store the final generated URL

WiFiClient espClient;

void goToSleep() {
  Serial.println("Going to sleep now");
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  digitalWrite(BLUE_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  digitalWrite(GREEN_LED_PIN, LOW);
  Serial.flush();
  esp_deep_sleep_start();
}

void displayColor(int redValue, int greenValue, int blueValue) {
  // Apply PWM (Pulse-Width Modulation) to each pin to control the color intensity
  analogWrite(RED_LED_PIN, redValue);
  analogWrite(GREEN_LED_PIN, greenValue);
  analogWrite(BLUE_LED_PIN, blueValue);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  unsigned long startMillis = millis();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");

    unsigned long currentMillis = millis();
    
    if (currentMillis - startMillis >= wifi_timeout) {
      displayColor(255, 255, 0); // Yellow
      delay(1000);
      goToSleep();
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void triggerPirResponse() {
  sprintf(signalMessage, "Motion Detected.");
  message_to_signal(signalMessage);
  delay(2000);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);

  gpio_wakeup_enable(WAKEUP_GPIO, GPIO_INTR_HIGH_LEVEL);
  esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);

  setup_wifi();
  
  // Dont need time since Signal Message has it
  // configTzTime(gmtOffsetString, ntpServer);
  triggerPirResponse();
  goToSleep();
}

void loop() {}

void message_to_signal(String message)
{
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);
  postData();
  delay(1000); // For LEDs
}

String urlencode(String str)
{
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield();
    }
    return encodedString;
}

void postData()
{
  int httpCode;
  HTTPClient http;
  http.begin(url);
  httpCode = http.POST(url);
  if (httpCode == 200) {
    displayColor(0, 255, 0);   // Green
    Serial.println("Sent ok."); 
  }
  else { 
    displayColor(255, 0, 0);   // Red
    Serial.println("Error."); 
  }
  http.end();
}