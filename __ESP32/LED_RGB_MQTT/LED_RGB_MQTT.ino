#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#define MAX 100 
// Normally set to 10, but loop in this program is lengthy so can reduce to 5
#define MAX_MQTT_CONN_ATTEMPTS 5

const int trigPin = 32;
const int echoPin = 35;

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

const char* mqtt_server = MQTT_ESP32_Server;
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_RGB";

// Use this to track retries
int num_mqtt_connection_attempts = 0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
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

void try_reconnect() {
  Serial.print("Attempting MQTT connection (Attempt #");
  Serial.print(num_mqtt_connection_attempts);
  Serial.print(")...");
  // Attempt to connect
  if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}
// Set up the rgb led names
uint8_t ledR = 0;
uint8_t ledG = 2;
uint8_t ledB = 4;

const boolean invert = true;  // set true if common anode, false if common cathode

uint8_t color = 0;         // a value from 0 to 255 representing the hue
uint32_t R, G, B;          // the Red Green and Blue color components
uint8_t brightness = 255;  // 255 is maximum brightness, but can be changed.  Might need 256 for common anode to fully turn off.

// the setup routine runs once when you press reset:
void setup() {
  Serial.begin(115200);
  delay(10);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // Initialize pins as LEDC channels
  // resolution 1-16 bits, freq limits depend on resolution, channel is automatically selected
  ledcAttach(ledR, 12000, 8);  // 12 kHz PWM, 8-bit resolution
  ledcAttach(ledG, 12000, 8);
  ledcAttach(ledB, 12000, 8);
}

// void loop runs over and over again
void loop() {
  if (!client.connected() && num_mqtt_connection_attempts < MAX_MQTT_CONN_ATTEMPTS) {
    num_mqtt_connection_attempts++;
    try_reconnect();
  }
  Serial.println("Send all LEDs a 255 and wait 2 seconds.");
  // If your RGB LED turns off instead of on here you should check if the LED is common anode or cathode.
  // If it doesn't fully turn off and is common anode try using 256.
  ledcWrite(ledR, 255);
  ledcWrite(ledG, 255);
  ledcWrite(ledB, 255);
  delay(2000);
  Serial.println("Send all LEDs a 0 and wait 2 seconds.");
  ledcWrite(ledR, 0);
  ledcWrite(ledG, 0);
  ledcWrite(ledB, 0);
  delay(2000);

  Serial.println("Starting color fade loop.");

  for (color = 0; color < 255; color++) {  // Slew through the color spectrum

    hueToRGB(color, brightness);  // call function to convert hue to RGB

    char full_message[MAX];
    snprintf(full_message, sizeof(full_message), "R: %u; G: %u; B: %u", R, G, B);
    client.publish("esp32/rgb", full_message);

    // write the RGB values to the pins
    ledcWrite(ledR, R);  // write red component to channel 1, etc.
    ledcWrite(ledG, G);
    ledcWrite(ledB, B);

    delay(100);  // full cycle of rgb over 256 colors takes 26 seconds
  }
}

// function to convert a color to its Red, Green, and Blue components.

void hueToRGB(uint8_t hue, uint8_t brightness) {
  uint16_t scaledHue = (hue * 6);
  uint8_t segment = scaledHue / 256;                     // segment 0 to 5 around the
                                                         // color wheel
  uint16_t segmentOffset = scaledHue - (segment * 256);  // position within the segment

  uint8_t complement = 0;
  uint16_t prev = (brightness * (255 - segmentOffset)) / 256;
  uint16_t next = (brightness * segmentOffset) / 256;

  if (invert) {
    brightness = 255 - brightness;
    complement = 255;
    prev = 255 - prev;
    next = 255 - next;
  }

  switch (segment) {
    case 0:  // red
      R = brightness;
      G = next;
      B = complement;
      break;
    case 1:  // yellow
      R = prev;
      G = brightness;
      B = complement;
      break;
    case 2:  // green
      R = complement;
      G = brightness;
      B = next;
      break;
    case 3:  // cyan
      R = complement;
      G = prev;
      B = brightness;
      break;
    case 4:  // blue
      R = next;
      G = complement;
      B = brightness;
      break;
    case 5:  // magenta
    default:
      R = brightness;
      G = complement;
      B = prev;
      break;
  }
}
