#include "Env.h"
#include <WiFi.h>
#include <HTTPClient.h>

#define MAX 100 

// ESP32_NOW
#include "esp_now.h"

// --- Topics
const int TOPIC_ID_KETTLE_THERMO = 1;
const int TOPIC_ID_SINK_THERMO   = 2;
// const int TOPIC_ID_KETTLE_SOUND  = 3;
// ---

typedef struct data {
  int topicId;
  int measurement;
} Data;

Data data;

// Vars to be used and overwritten
int sensorMeasurement = 0;
int randomValue = 0;
int randomIndex = 0;
int arraySize = 0;

int kettleRandoms[] = {12, 100, 144, 50, 65, 30, 500, 450, 6};
int sinkRandoms[] = {12, 91, 88, 31, 65, 30, 433, 6, 11};

// Receiver MAC = Station MAC: 00:4B:12:F2:54:10 (Network Hub Rx 1)
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0xF2, 0x54, 0x10}; // Hex

void setup_wifi() {
  WiFi.mode(WIFI_STA); // ESP Now requires WiFi header. Goes into STA mode.
  WiFi.disconnect();
}

void setup() {
  Serial.begin(115200);
  setup_wifi();

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

  // Seed the random number generator using the current time
  // This should only be called once per program execution
  srand(time(NULL));
}

int randomFromList(int topicId) {
  int (*ptr)[9];
  if (topicId == TOPIC_ID_KETTLE_THERMO) {
    ptr = &kettleRandoms;
  } else if (topicId == TOPIC_ID_SINK_THERMO) {
    ptr = &sinkRandoms;
  } else {
    return 0;
  }
  arraySize = sizeof((*ptr)) / sizeof((*ptr)[0]); // Determine the size of the array
  randomIndex = rand() % arraySize; // Generate random index within the bounds of the array
  randomValue = (*ptr)[randomIndex]; // Select element at the random index
  return randomValue;
}

void takeReading(int topicId) {
  data.topicId = topicId;
  sensorMeasurement = randomFromList(topicId);
  data.measurement = sensorMeasurement;
  Serial.printf("Sending ESP NOW message. Topic ID: %d || Measurement: %d\n", topicId, sensorMeasurement);
  esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));
}

void loop() {
  takeReading(TOPIC_ID_KETTLE_THERMO);
  delay(2000);
  takeReading(TOPIC_ID_SINK_THERMO);
  delay(2000);
}

