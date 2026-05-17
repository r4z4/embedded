#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#include <IRremote.hpp>

#define MAX_MQTT_CONN_ATTEMPTS 5
#define MAX 16 // Max Message Length
#define IR_RECV_PIN 36

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

const char* mqtt_server = "192.168.1.139";
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_IR_Recv";

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
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  if (!client.connected()) {
    try_reconnect();
  }
  IrReceiver.begin(IR_RECV_PIN, ENABLE_LED_FEEDBACK);
}


void loop() {
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
