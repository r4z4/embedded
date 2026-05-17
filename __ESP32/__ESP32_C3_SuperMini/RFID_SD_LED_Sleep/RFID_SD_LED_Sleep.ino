#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
//#include <MFRC522DriverI2C.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include "driver/rtc_io.h"

#define USE_EXT0_WAKEUP 1
#define WAKEUP_GPIO GPIO_NUM_3 // Cannot use SDA Pin
#define WAKEUP_GPIO_BITMASK (1ULL << GPIO_NUM_3)

#define CS_PIN 1
#define SDA_PIN 8
#define RED_LED_PIN 10
#define BLUE_LED_PIN 20
#define GREEN_LED_PIN 21 // R and G reversed

// byte allowedUID[] = {0xA3, 0xB2, 0x13, 0x32}; // Orange UID
char allowedString[10] = "a3b21332";

// Learn more about using SPI/I2C or check the pin assigment for your board: https://github.com/OSSLibraries/Arduino_MFRC522v2#pin-layout
MFRC522DriverPinSimple ss_pin(SDA_PIN);

MFRC522DriverSPI driver{ss_pin}; // Create SPI driver
//MFRC522DriverI2C driver{};     // Create I2C driver
MFRC522 mfrc522{driver};         // Create MFRC522 instance

char logString[80];

void goToSleep() {
  Serial.println("Going to sleep now");
  // WiFi.disconnect(true);
  // WiFi.mode(WIFI_OFF);
  // analogWrite?
  analogWrite(RED_LED_PIN, 0);
  analogWrite(GREEN_LED_PIN, 0);
  analogWrite(BLUE_LED_PIN, 0);
  Serial.flush();
  esp_deep_sleep_start();
}

void displayColor(int redValue, int greenValue, int blueValue) {
  // Apply PWM (Pulse-Width Modulation) to each pin to control the color intensity
  analogWrite(RED_LED_PIN, redValue);
  analogWrite(GREEN_LED_PIN, greenValue);
  analogWrite(BLUE_LED_PIN, blueValue);
}

void setup() {
  Serial.begin(115200);  // Initialize serial communication
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BLUE_LED_PIN, OUTPUT);

  gpio_wakeup_enable(WAKEUP_GPIO, GPIO_INTR_HIGH_LEVEL);
  esp_deep_sleep_enable_gpio_wakeup(WAKEUP_GPIO_BITMASK, ESP_GPIO_WAKEUP_GPIO_HIGH);

  while (!Serial);       // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4).
  
  mfrc522.PCD_Init();    // Init MFRC522 board.
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);	// Show details of PCD - MFRC522 Card Reader details.

	// Set up SD
    if(!SD.begin(CS_PIN)){
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();

  if(cardType == CARD_NONE){
    Serial.println("No SD card attached");
    return;
  }

  Serial.print("SD Card Type: ");
  if(cardType == CARD_MMC){
    Serial.println("MMC");
  } else if(cardType == CARD_SD){
    Serial.println("SDSC");
  } else if(cardType == CARD_SDHC){
    Serial.println("SDHC");
  } else {
    Serial.println("UNKNOWN");
  }

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);

  // createDir(SD, "/mydir");
  // Only write if not present
  writeFile(SD, "/log.txt", "Log file for RFID readings\n");
  Serial.println(F("Scan PICC to see UID"));
}

int loopsWaiting = 0;

void loop() {
	// Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
	if (!mfrc522.PICC_IsNewCardPresent()) {
    displayColor(255, 255, 0); // Yellow
    loopsWaiting++;
    if (loopsWaiting > 3000) {
      goToSleep();
    }
		return;
	}

	// Select one of the cards.
	if (!mfrc522.PICC_ReadCardSerial()) {
    displayColor(255, 255, 0); // Yellow
    loopsWaiting++;
    if (loopsWaiting > 3000) {
      goToSleep();
    }
		return;
	}

  // Got a reading
  loopsWaiting = 0;
  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, (mfrc522.uid));
  Serial.println();

  // Save the UID on a String variable
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }

  // Compare str1 and str2
  if (strcmp(uidString.c_str(), allowedString) == 0) {
    Serial.print("Valid\n");
    displayColor(0, 255, 0);   // Green
    delay(200);
  } else {
    Serial.print("Invalid\n");
    displayColor(255, 0, 0);   // Red
    delay(200);
  }

  Serial.println(uidString);
  sprintf(logString, "%s\n", uidString);
  appendFile(SD, "/log.txt", logString);
}

void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file){
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)){
      Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}