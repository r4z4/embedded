#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include  <Adafruit_BMP280.h>

#define TOUCH_PIN_OFF 32 // T9
#define TOUCH_PIN_ON 33 // T8
#define TOUCH_S1 27 // T7
#define TOUCH_S2 14 // T6
#define TOUCH_S3 13 // T4
#define THRESHOLD 20

char s1Message[80];
char s2Message[80];
char s3Message[80];

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Adafruit_BMP280  bmp;

RTC_DATA_ATTR int bootCount = 0;
int offTouch = 0;
int s1Touch = 0;
int s2Touch = 0;
int s3Touch = 0;

touch_pad_t touchPin;

void sleepDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYOFF);
}

void wakeDisplay(Adafruit_SSD1306* display) {
  display->ssd1306_command(SSD1306_DISPLAYON);
}


void setup() {
  Serial.begin(115200);
  delay(1000);
  ++bootCount;
  Serial.println("Boot Number: " + String(bootCount));
  //Print the wakeup reason for ESP32 and touchpad too
  print_wakeup_reason();
  print_wakeup_touchpad();
  touchSleepWakeUpEnable(TOUCH_PIN_ON, THRESHOLD);
  wakeDisplay(&display);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.display();
  bmp.begin(0x76);
  delay(2000);
}

void loop() {
  offTouch = touchRead(TOUCH_PIN_OFF); // touchRead when using actual pin
  s1Touch = touchRead(TOUCH_S1); // touchRead when using actual pin
  s2Touch = touchRead(TOUCH_S2); // touchRead when using actual pin
  s3Touch = touchRead(TOUCH_S3); // touchRead when using actual pin
  if( offTouch > 5 && offTouch < THRESHOLD ){
    Serial.println("Off pressed. Going to Sleep.");
    // WiFi.disconnect(true);
    // WiFi.mode(WIFI_OFF);
    sleepDisplay(&display);
    Serial.flush();
    esp_deep_sleep_start(); // If looping we are awake
  }

  if ( s1Touch > 5 && s1Touch < THRESHOLD ) {
    handleSensor1Touch();
  }
  if ( s2Touch > 5 && s2Touch < THRESHOLD ) {
    handleSensor2Touch();
  }
  if ( s3Touch > 5 && s3Touch < THRESHOLD ) {
    handleSensor3Touch();
  }
}

void handleSensor1Touch() {
  // BMP280
  // float rawTemp = bmp.readTemperature();
  // Serial.print("Raw Temp: ");
  // Serial.println(rawTemp);
  float T = bmp.readTemperature()*9/5  + 32; // Read temperature in C and conversion to F
  float P = bmp.readPressure()/3386.39; //Read Pressure in Pa and conversion to inches of mercury
  float A  = bmp.readAltitude(1019.66)*3.28084; //Calculating the Altitude, the "1019.66"  is the pressure in(hPa) at sea level at day in your region
  snprintf(s1Message, sizeof(s1Message), "BMP => Temp: %f, Pres: %f, Alt: %f", T, P, A);
  printToScreen(s1Message);
}

void handleSensor2Touch() {
  // Ultrasonic (For Now)
  int redValue = 2;
  int greenValue = 9;
  int blueValue = 7;
  snprintf(s2Message, sizeof(s2Message), "Sensor 2: %u, %u, %u", redValue, greenValue, blueValue);
  printToScreen(s2Message);
}

void handleSensor3Touch() {
  snprintf(s3Message, sizeof(s3Message), "Sensor 3");
  Serial.println(s3Message);
  printToScreen(s3Message);
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

void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch (wakeup_reason) {
    case ESP_SLEEP_WAKEUP_EXT0:     Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1:     Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER:    Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP:      Serial.println("Wakeup caused by ULP program"); break;
    default:                        Serial.printf("Wakeup was not caused by deep sleep: %d\n", wakeup_reason); break;
  }
}

void print_wakeup_touchpad() {
  int touchPin = esp_sleep_get_touchpad_wakeup_status();

#if CONFIG_IDF_TARGET_ESP32
  switch (touchPin) {
    case 0:  Serial.println("Touch detected on GPIO 4"); break;
    case 1:  Serial.println("Touch detected on GPIO 0"); break;
    case 2:  Serial.println("Touch detected on GPIO 2"); break;
    case 3:  Serial.println("Touch detected on GPIO 15"); break;
    case 4:  Serial.println("Touch detected on GPIO 13"); break;
    case 5:  Serial.println("Touch detected on GPIO 12"); break;
    case 6:  Serial.println("Touch detected on GPIO 14"); break;
    case 7:  Serial.println("Touch detected on GPIO 27"); break;
    case 8:  Serial.println("Touch detected on GPIO 33"); break;
    case 9:  Serial.println("Touch detected on GPIO 32"); break;
    default: Serial.println("Wakeup not by touchpad"); break;
  }
#else
  if (touchPin < TOUCH_PAD_MAX) {
    Serial.printf("Touch detected on GPIO %d\n", touchPin);
  } else {
    Serial.println("Wakeup not by touchpad");
  }
#endif
}