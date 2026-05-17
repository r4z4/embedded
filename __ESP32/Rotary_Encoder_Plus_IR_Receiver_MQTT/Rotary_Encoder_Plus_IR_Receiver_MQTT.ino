#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <ezButton.h>  // the library to use for SW pin
#include <stdio.h>
#include <stdlib.h>
#include <IRremote.hpp>

#define MAX 100
#define CLK_PIN 25 // ESP32 pin GPIO25 connected to the rotary encoder's CLK pin
#define DT_PIN  26 // ESP32 pin GPIO26 connected to the rotary encoder's DT pin
#define SW_PIN  27 // ESP32 pin GPIO27 connected to the rotary encoder's SW pin

#define DIRECTION_CW  0   // clockwise direction
#define DIRECTION_CCW 1  // counter-clockwise direction

#define SHORT_PRESS_TIME 1000 // 1000 milliseconds
#define LONG_PRESS_TIME  1000 // 1000 milliseconds

#define IR_MAX 16 // Max Message Length
#define IR_RECV_PIN 36

#define MAX_MQTT_CONN_ATTEMPTS 5

// Use this to track retries
int num_mqtt_connection_attempts = 0;

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

const char* mqtt_client_name = "ESP32_Rotary_Plus_IR_Receiver";

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

void try_reconnect() {
  while (num_mqtt_connection_attempts < MAX_MQTT_CONN_ATTEMPTS) {
    num_mqtt_connection_attempts++;
    Serial.print("Attempting MQTT connection (Attempt #");
    Serial.print(num_mqtt_connection_attempts);
    Serial.print(")...");
    // Attempt to connect
    if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
      Serial.println("connected");
      break;
    } else {
      Serial.print("failed, rc=");
      Serial.println(client.state());
      delay(2000);
      try_reconnect();
    }
  }
}

// ezButton button(SW_PIN);  // create ezButton object that attach to pin 7;

void decode_apple(int command, char* pCode, int buffersize) {
  if (!pCode || buffersize < 1) {
    return; // Bad input.
  }
  const char *var;
  Serial.println(command);
  // if (IrReceiver.decodedIRData.command == 0x10) {
  switch (command) {
    case 0x8: // L
      var = "left";
      break;
    case 0xB: // U
      var = "up";
      break;
    case 0x7: // R
      var = "right";
      break;
    case 0xD: // D
      var = "down";
      break;
    case 0x2: // Menu
      var = "menu";
      break;
    case 0x5E: // Play
      var = "play";
      break;
    case 0x5D: // Center
      var = "center";
      break;
    case 0x4: // Gets sent after pressing Center and Play btns
      var = "extra";
      break;
    default:
      var = "default";
      break;
  }
  Serial.print("var = ");
  Serial.println(var);
  strncpy(pCode, var, buffersize-1);
  pCode[buffersize-1] = '\0';
}

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
  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
  // read the initial state of the rotary encoder's CLK pin
  prev_CLK_state = digitalRead(CLK_PIN);
}

void loop() {
  button.loop();  // MUST call the loop() function first
  state_changed = 0;
  press = 0;
  if (button.isPressed())
      pressedTime = millis();

    if (button.isReleased()) {
      releasedTime = millis();

      long pressDuration = releasedTime - pressedTime;

      if ( pressDuration < SHORT_PRESS_TIME ) {
        press = 1;
      } else if ( pressDuration > LONG_PRESS_TIME ) {
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
  }

  if (state_changed == 1 || press > 0) {
    char rot_message[MAX];
    // %d = signed
    snprintf(rot_message, sizeof(rot_message), "Counter: %d; Dir: %u; Btn: %u", counter, direction, press);
    Serial.println(rot_message);
    // Publish the string
    client.publish("esp32/rotary_encoder", rot_message);
  }

  prev_CLK_state = CLK_state; // Save last CLK state

  if (IrReceiver.decode()) {
    // Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX); // Print "old" raw data
    // Serial.println(IrReceiver.decodedIRData.command);
    // IrReceiver.printIRResultShort(&Serial); // Print complete received data in one line
    char code[IR_MAX]; // 16 bytes allocated here on the stack.
    decode_apple(IrReceiver.decodedIRData.command, code, sizeof(code));
    if (strcmp(code, "default") == 0 || strcmp(code, "extra") == 0) {
      Serial.print("Default or Extra");
    } else {
      Serial.print("Pressed: ");
      Serial.println(code);
      char ir_message[MAX];
      snprintf(ir_message, sizeof(ir_message), "%s", code);
      client.publish("esp32/ir", ir_message);
    }
    IrReceiver.resume(); // Enable receiving of the next value
  }
}
