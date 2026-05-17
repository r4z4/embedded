// Board: ESP32_C3_SuperMini

#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <HTTPClient.h>
#include "time.h"

#define MAX 100 
const int trigPin = 1;
const int echoPin = 2;

const char *ssid = WIFI_SSID; // Enter your WiFi name
const char *password = WIFI_PASS;  // Enter WiFi password

// ESP32_NOW
#include "esp_now.h"

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data data;

char* topic = "ESP32_HC_SR04";

// Receiver MAC = Station MAC: 00:4B:12:2E:86:94 (ESP32 MQTT Broker)
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x2E, 0x86, 0x94}; // Hex

// Signal
#define THRESHOLD_IN 20
const char* ntpServer = "pool.ntp.org";
const char* gmtOffsetString = "CST6CDT,M3.2.0,M11.1.0"; // https://github.com/nayarsystems/posix_tz_db

String apiKey = SIGNAL_API_KEY;
String phone_number = SIGNAL_PHONE_NUMBER;

char currentTime[20];
char signalMessage[50];
String url; // URL String used to store the final generated URL

// Sleep
int TIME_TO_SLEEP = 300;
unsigned long long uS_TO_S_FACTOR = 1000000; // 300 Seconds
RTC_DATA_ATTR int bootCount = 0;

// MQTT
// const char* mqtt_server = "192.168.1.139";
// const char* mqtt_username = "admin"; // Auth Token
// const char* mqtt_password = "adminpwd";

// const char* mqtt_client_name = "ESP32_HC_SR04";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  // initialize serial communication:
  Serial.begin(115200);
  ++bootCount;
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Wake up source
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  setup_wifi();
  // client.setServer(mqtt_server, 1883);
  // client.setCallback(callback); For receiving messages
  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW Init Failed");
    return;
  }
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  configTzTime(gmtOffsetString, ntpServer);

  takeReading();

  // Go to sleep
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
  Serial.flush();
  // Enable sleep
  esp_deep_sleep_start();
}

void takeReading() {
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH pulse
  // whose duration is the time (in microseconds) from the sending of the ping
  // to the reception of its echo off of an object.
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  if (inches < THRESHOLD_IN) {
    getLocalTime();
    sprintf(signalMessage, "Garage is open. Time is: %s", currentTime);
    message_to_signal(signalMessage);
  }

  char full_message[MAX];
  snprintf(full_message, sizeof(full_message), "In: %lu; Cm: %lu", inches, cm);
  // Publish the string
  // client.publish("esp32/sensor_data_hc_sr04", full_message);

  data.topic = topic;
  data.entry = full_message;

  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
}

void loop() {}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}

void message_to_signal(String message)
{
  url = "https://api.callmebot.com/signal/send.php?phone=" + phone_number + "&apikey=" + apiKey + "&text=" + urlencode(message);
  postData();
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
  if (httpCode == 200) { Serial.println("Sent ok."); }
  else { Serial.println("Error."); }
  http.end();
}
