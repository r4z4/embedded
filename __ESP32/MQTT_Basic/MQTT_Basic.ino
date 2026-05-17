// #include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>

const char *ssid = "wifi_ssid"; // Enter your WiFi name
const char *password = "wifi_pass";  // Enter WiFi password

// Add your MQTT Broker IP address, example:
// const char* mqtt_server = "Local IP";
// const char* mqtt_server = "broker.hivemq.com";
const char* mqtt_server = "mqtt.flespi.io";
const char* mqtt_username = "auth_token"; // Auth Token
const char* mqtt_password = ""; 

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

//uncomment the following lines if you're using SPI
/*#include <SPI.h>
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5*/

void setup() {
  Serial.begin(115200);
  // default settings
  // (you can also pass in a Wire library object like &Wire2)
  //status = bme.begin();  
  // if (!bme.begin(0x76)) {
  //   Serial.println("Could not find a valid BME280 sensor, check wiring!");
  //   while (1);
  // }
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  // pinMode(ledPin, OUTPUT);
}

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

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32_Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
      client.publish("esp32/hello", "Finally made it!");
      Serial.println("Sent it");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.publish("esp32/hello", "Message #1");
  Serial.println("Sent #1");
  delay(2000);
  client.publish("esp32/hello", "Message #2");
  Serial.println("Sent #2");
  delay(2000);
  client.publish("esp32/hello", "Message #3");
  Serial.println("Sent #3");
  delay(5000);
}