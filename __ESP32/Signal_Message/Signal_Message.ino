#include <WiFi.h>
#include <HTTPClient.h>
#include "Env.h"
#include "time.h"

#define TOUCH_PIN 32
#define THRESHOLD 20
int capacitiveTouch = 0;

const char* ntpServer = "pool.ntp.org";
const char* gmtOffsetString = "CST6CDT,M3.2.0,M11.1.0"; // https://github.com/nayarsystems/posix_tz_db

const char* ssid = WIFI_SSID;             
const char* password =  WIFI_PASS;

String apiKey = SIGNAL_API_KEY;
String phone_number = SIGNAL_PHONE_NUMBER;

char currentTime[20];
char signalMessage[50];
String url; // URL String used to store the final generated URL

void setup()
{
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to the WiFi network");

  configTzTime(gmtOffsetString, ntpServer);
  // getLocalTime();

  // sprintf(signalMessage, "Hello from ESP32. Time is: %s", currentTime);

  // message_to_signal(signalMessage);
}

void loop()
{
  capacitiveTouch = touchRead(TOUCH_PIN);
  
  if (capacitiveTouch > 5 && capacitiveTouch < THRESHOLD) {
    Serial.print("Touched: ");
    Serial.println(capacitiveTouch);
    getLocalTime();
    sprintf(signalMessage, "Hello from ESP32. Time is: %s", currentTime);
    message_to_signal(signalMessage);
  }
}

void message_to_signal(String message)
{
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);
  postData();
}

void postData()
{
  int httpCode;
  HTTPClient http;
  http.begin(url);
  httpCode = http.POST(url);
  if (httpCode == 200) { Serial.println("Sent ok."); }
  else { Serial.println("Error."); }
  http.end();
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

void getLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  sprintf(
    currentTime,
    "%04d-%02d-%02d %02d:%02d:%02d",
    timeinfo.tm_year+1900,
    timeinfo.tm_mon+1, // tm_mon is value 0-11
    timeinfo.tm_mday,
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
  );
  Serial.print("Now is: ");
  Serial.println(currentTime);
}