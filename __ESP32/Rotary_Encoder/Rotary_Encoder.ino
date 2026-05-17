#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ezButton.h>  // the library to use for SW pin
#include <stdio.h>
#include <stdlib.h>

#define MAX 100
#define CLK_PIN 25 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  26 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  27 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

#define SHORT_PRESS_TIME 1000 // 1000 milliseconds
#define LONG_PRESS_TIME  1000 // 1000 milliseconds

ezButton button(SW_PIN);

// Variables will change:
int lastBtnState = LOW;  // the previous state from the input pin
int currentBtnState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
int press;

int counter = 0;
int direction = DIRECTION_CW;
int CLK_state;
int prev_CLK_state;
int state_changed;

// MQTT
const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS; 

const char* mqtt_client_name = "ESP32_Rotary";

WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi(const char* ssid, const char* password) {
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
    if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

// ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;

void setup() {
  Serial.begin(9600);
  setup_wifi(WIFI_SSID, WIFI_PASS);
  client.setServer(mqtt_server, 1883);

  // configure encoder pins as inputs
  pinMode(CLK_PIN, INPUT);
  pinMode(DT_PIN, INPUT);
  button.setDebounceTime(50);  // set debounce time to 50 milliseconds
  if (!client.connected()) {
    try_reconnect();
  }
  // read the initial state of the rotary encoder's CLK pin
  prev_CLK_state = digitalRead(CLK_PIN);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  button.loop();  // MUST call the loop() function first
  state_changed = 0;
  press = 0;
  if (button.isPressed())
      pressedTime = millis();

    if (button.isReleased()) {
      releasedTime = millis();

      long pressDuration = releasedTime - pressedTime;

      if ( pressDuration < SHORT_PRESS_TIME ) {
        Serial.println("A short press is detected");
        press = 1;
      } else if ( pressDuration > LONG_PRESS_TIME ) {
        Serial.println("A long press is detected");
        press = 2;
      }
    }
  // read the current state of the rotary encoder's CLK pin
  CLK_state = digitalRead(CLK_PIN);

  // If the state of CLK is changed, then pulse occurred
  // React to only the rising edge (from LOW to HIGH) to avoid double count
  if (CLK_state != prev_CLK_state && CLK_state == HIGH) {
    state_changed = 1;
    // if the DT state is HIGH
    // the encoder is rotating in counter-clockwise direction => decrease the counter
    if (digitalRead(DT_PIN) == HIGH) {
      counter++;
      if (counter >= 100) {
        counter = 100; // Max out at 100
      }
      direction = DIRECTION_CW;
    } else {
      counter--;
      if (counter <= -100) {
        counter = -100; // Min out at -100
      }
      direction = DIRECTION_CCW;
    }

    Serial.print("Rotary Encoder:: direction: ");
    if (direction == DIRECTION_CW)
      Serial.print("Clockwise");
    else
      Serial.print("Counter-clockwise");

    Serial.print(" - count: ");
    Serial.println(counter);
  }

  if (state_changed == 1 || press > 0) {
    char full_message[MAX];
    // %d = signed
    snprintf(full_message, sizeof(full_message), "Counter: %d; Dir: %u; Btn: %u", counter, direction, press);
    // Publish the string
    client.publish("esp32/rotary_encoder", full_message);
  }

  // save last CLK state
  prev_CLK_state = CLK_state;

}
