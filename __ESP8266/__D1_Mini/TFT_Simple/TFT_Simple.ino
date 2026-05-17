#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int SDA_PIN = 4;
const int SCL_PIN = 5;

int testNum = 0;
char sensorMessage[10];

void createMessage(int type) {
  snprintf(sensorMessage, sizeof(sensorMessage), "Test#: %d", type);
  Serial.println(sensorMessage);
  printToScreen(sensorMessage);
}

void printToScreen(char* printStr) {
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(printStr);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display(); // Display the cleared buffer
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
}

void loop() {
  testNum++;
  createMessage(testNum);
  delay(2000);
}