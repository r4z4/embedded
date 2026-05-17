#include <Wire.h>
#include <AHTxx.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
char failMsg[10];
char failMsg2[10];
float humValue;  
float tempValue;                              
AHTxx aht20(AHTXX_ADDRESS_X38, AHT2x_SENSOR); 

const int SDA_PIN = 4;
const int SCL_PIN = 5;

void triggerSensorResponse() {
  /* DEMO - 1, every temperature or humidity call will read 6-bytes over I2C, total 12-bytes */
  Serial.println();
  Serial.println(F("DEMO 1: read 12-bytes"));

  tempValue = aht20.readTemperature(); //read 6-bytes via I2C, takes 80 milliseconds

  Serial.print(F("Temperature...: "));
  
  if (tempValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(tempValue);
    Serial.println(F(" +-0.3C"));
  }
  else
  {
    printStatus(); //print temperature command status

    if   (aht20.softReset() == true) Serial.println(F("reset success")); //as the last chance to make it alive
    else                             Serial.println(F("reset failed"));
  }

  delay(2000); //measurement with high frequency leads to heating of the sensor, see NOTE

  humValue = aht20.readHumidity(); //read another 6-bytes via I2C, takes 80 milliseconds

  Serial.print(F("Humidity......: "));
  
  if (humValue != AHTXX_ERROR) //AHTXX_ERROR = 255, library returns 255 if error occurs
  {
    Serial.print(humValue);
    Serial.println(F(" +-2%"));
  }
  else
  {
    printStatus(); 
  }

  delay(2000); //measurement with high frequency leads to heating of the sensor, see NOTE

  char temp_message[20];
  char hum_message[20];
  snprintf(temp_message, sizeof(temp_message), "Temp: %.1f", tempValue);
  snprintf(hum_message, sizeof(hum_message), "Hum: %.1f", humValue);
  // Publish the string
  Serial.println(temp_message);
  Serial.println(hum_message);
  printToScreen(temp_message, hum_message);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  // Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // Initialize with I2C address 0x3C (use 0x3D if needed)
  while (aht20.begin() != true) //for ESP-01 use aht20.begin(0, 2);
  {
    Serial.println(F("AHT2x not connected or fail to load calibration coefficient")); //(F()) save string to flash & keeps dynamic memory free
    sprintf(failMsg, "Failure");
    sprintf(failMsg2, "Failure");
    printToScreen(failMsg, failMsg);
    delay(5000);
  }

  Serial.println(F("AHT20 OK"));
}

void printToScreen(char* printStr, char* printStr2) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(2);
  display.setCursor(0, 20);
  display.println(printStr);
  display.println(printStr2);
  display.display();
  delay(3000);
  display.clearDisplay();
  display.display(); // Display the cleared buffer
}

void loop() {
  triggerSensorResponse();
  delay(1000);
}


void printStatus()
{
  switch (aht20.getStatus())
  {
    case AHTXX_NO_ERROR:
      Serial.println(F("no error"));
      break;

    case AHTXX_BUSY_ERROR:
      Serial.println(F("sensor busy, increase polling time"));
      break;

    case AHTXX_ACK_ERROR:
      Serial.println(F("sensor didn't return ACK, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_DATA_ERROR:
      Serial.println(F("received data smaller than expected, not connected, broken, long wires (reduce speed), bus locked by slave (increase stretch limit)"));
      break;

    case AHTXX_CRC8_ERROR:
      Serial.println(F("computed CRC8 not match received CRC8, this feature supported only by AHT2x sensors"));
      break;

    default:
      Serial.println(F("unknown status"));    
      break;
  }
}