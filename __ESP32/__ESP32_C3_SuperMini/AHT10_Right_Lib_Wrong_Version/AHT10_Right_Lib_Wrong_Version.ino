#include "driver/rtc_io.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

#define DHTPIN 1     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

#define USE_EXT0_WAKEUP 1
#define WAKEUP_GPIO GPIO_NUM_4
#define WAKEUP_GPIO_BITMASK (1ULL << GPIO_NUM_4)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

char sensorMessage[50];

void sleepDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYOFF);
}

void wakeDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYON);
}


void goToSleep() {
  Serial.println("Going to sleep now");
  Serial.flush();
  sleepDisplay(&display);
  esp_deep_sleep_start();
}

void triggerSensorResponse() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  char temp_message[10];
  char hum_message[10];
  snprintf(temp_message, sizeof(temp_message), "Temp: %.1f", f);
  snprintf(hum_message, sizeof(hum_message), "Hum: %.1f", h);
  // Publish the string
  Serial.println(temp_message);
  Serial.println(hum_message);
  printToScreen(temp_message, hum_message);
}

void printToScreen(char* printStr, char* printStr2) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(printStr);
  display.println(printStr2);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display(); // Display the cleared buffer
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  wakeDisplay(&display);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  
  dht.begin();
  gpio_wakeup_enable(WAKEUP_GPIO, GPIO_INTR_HIGH_LEVEL);
  esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);

  triggerSensorResponse();
  goToSleep();
}

void loop() {}