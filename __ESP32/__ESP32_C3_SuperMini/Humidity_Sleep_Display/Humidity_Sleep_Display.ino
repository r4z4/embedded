#include "driver/rtc_io.h"
#include <Adafruit_AHTX0.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define USE_EXT0_WAKEUP 1
#define WAKEUP_GPIO GPIO_NUM_4
#define WAKEUP_GPIO_BITMASK (1ULL << GPIO_NUM_4)

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_AHTX0 aht;
Adafruit_Sensor *aht_humidity, *aht_temp;

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
  // Initialize AHT sensor
  if (!aht.begin()) {
    Serial.println("Failed to find AHT10/AHT20 chip");
    while (1) {
      delay(10);
    }
  }
  sensors_event_t humidity;
  sensors_event_t temp;
  aht_humidity->getEvent(&humidity);
  aht_temp->getEvent(&temp);

  Serial.print("Humidity: ");
  Serial.println(humidity.relative_humidity);

  snprintf(sensorMessage, sizeof(sensorMessage), "Temp: %f, Hum: %f", temp.temperature, humidity.relative_humidity);
  printToScreen(sensorMessage);
  // html += "<p>Temperature: " + String(temp.temperature) + " deg C</p>";
  // html += "<p>Humidity: " + String(humidity.relative_humidity) + " % rH</p>";
}

void printToScreen(char* printStr) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(20, 20);
  display.println(printStr);
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

  gpio_wakeup_enable(WAKEUP_GPIO, GPIO_INTR_HIGH_LEVEL);
  esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);

  triggerSensorResponse();
  goToSleep();
}

void loop() {}