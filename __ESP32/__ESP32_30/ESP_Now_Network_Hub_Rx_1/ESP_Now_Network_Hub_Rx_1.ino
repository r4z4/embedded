#include <WiFi.h>
#include "ESP32_NOW.h"

// --- Topics
const int TOPIC_ID_KETTLE_THERMO = 1;
const int TOPIC_ID_SINK_THERMO   = 2;
const int TOPIC_ID_KETTLE_SOUND  = 3;
// ---

// --- Outputs
// 34-39 == Input Only
const int THERMO_1_LED_PIN = 32; // Kettle Thermostat
const int THERMO_2_LED_PIN = 12; // Sink Thermostat
// const int THERMO_3_LED_PIN = 13; // Kettle Sound
const int BUZZER_PIN = 13; // 34-39 == Input Only
// ---

typedef struct HubData {
  int topicId;
  int measurement;
} HubData;

HubData received;

#define USE_EXT0_WAKEUP 1
#define WAKEUP_GPIO GPIO_NUM_4
#define WAKEUP_GPIO_BITMASK (1ULL << GPIO_NUM_4)

// RTC_DATA_ATTR places this variable in slow RTC memory, 
// so it survives deep sleep.
RTC_DATA_ATTR bool isAwake = false;

void goToSleep() {
  Serial.println("Going to sleep now");
  Serial.flush();
  isAwake = false;
  // sleepDisplay(&display);
  // // Re-enable wakeup for when you want to wake it up again
  // esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);
  esp_deep_sleep_start();
}



void soundBuzzer() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
}

void flashLED(int ledPin) {
  digitalWrite(ledPin, HIGH);
  delay(1000);
  digitalWrite(ledPin, LOW);
}

void setLED(int topicId, int measurement) {
  Serial.printf("Topic ID: %d || Measurement: %d\n", topicId, measurement);
  if (topicId == TOPIC_ID_KETTLE_THERMO) {
    if (measurement > 50) {
      soundBuzzer(); // Sound Buzzer before LED
      flashLED(THERMO_1_LED_PIN);
    } else {
      digitalWrite(THERMO_1_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else if (topicId == TOPIC_ID_SINK_THERMO) {
    if (measurement < 32) {
      soundBuzzer();
      flashLED(THERMO_2_LED_PIN);
    } else {
      digitalWrite(THERMO_2_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
    }
  } else {
      digitalWrite(THERMO_1_LED_PIN, LOW);
      digitalWrite(THERMO_2_LED_PIN, LOW);
      digitalWrite(BUZZER_PIN, LOW);
  }
}

void handleMeasurement(int topicId, int measurement) {
  if (topicId == TOPIC_ID_KETTLE_THERMO) {
    setLED(topicId, measurement);
  } else if (topicId == TOPIC_ID_SINK_THERMO) {
    setLED(topicId, measurement);
  }
}

void onReceive(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
  if (len == sizeof(HubData)) {
    Serial.println("Received Message");
    memcpy(&received, incomingData, sizeof(received));
    handleMeasurement(received.topicId, received.measurement);
  }
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Check if we just woke up from deep sleep
  esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();

  
  if (wakeup_reason == ESP_SLEEP_WAKEUP_GPIO) {
    if (isAwake) {
      // It was already awake, so this touch means "go to sleep"
      Serial.println("Sensor touched: Going to sleep");
      // Optional: Add debounce delay here if necessary
      goToSleep();
    } else {
      // It was asleep, now it's awake
      Serial.println("Sensor touched: Waking up");
      isAwake = true;
    }
  } else {
    // Normal power-on (reset button), set default state
    Serial.println("Normal Boot");
    isAwake = false; // Or true, depending on your preferred default
  }

  pinMode(THERMO_1_LED_PIN, OUTPUT);
  pinMode(THERMO_2_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  gpio_wakeup_enable(WAKEUP_GPIO, GPIO_INTR_HIGH_LEVEL);
  esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW Init Failed");
    return;
  }
  if (isAwake) {
    esp_now_register_recv_cb(onReceive);
  }
}

void loop() {
  // // If active, check if the pin is still HIGH to trigger sleep
  // if (isAwake && digitalRead(WAKEUP_GPIO_PIN) == HIGH) {
  //   // Optional: Add debounce/long-press detection here
  //   delay(500); 
  //   goToSleep();
  // }
}