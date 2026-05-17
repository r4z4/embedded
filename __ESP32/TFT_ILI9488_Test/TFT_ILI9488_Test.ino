#include <TFT_eSPI.h> // Graphics and font library for ILI9341 driver chip
#include <SPI.h>
#include "Env.h"

#ifndef TOUCH_CS
#include <TFT_eTouch.h>
#endif

#define THRESHOLD 20

int16_t x, y;

// Edit User_Setup_Select to ensure right settings.  Need to uncomment
// Line stating to load default User_Setup.h and Uncomment line that
// Will load the 303_User_Setup for ILI9488_Parallel
TFT_eSPI tft = TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
// Note that yield() or delay(0) must be called in long duration for/while
// loops to stop the ESP8266 watchdog triggering.

#ifndef TOUCH_CS
TFT_eTouch<TFT_eSPI> touch(tft, TFT_ETOUCH_CS, 0xff, TFT_eSPI::getSPIinstance());
#endif

#ifdef TOUCH_CS
#define CALIBRATION_FILE "/TouchCalibData4"
#else
#define CALIBRATION_FILE "/TFT_eTouch.cal"
#endif

unsigned long connCt = 0;
unsigned long subCt = 0;

void printScreen() {
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(TFT_WHITE,TFT_BLACK);  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.print("Connections = "); tft.println(connCt); 
  tft.println(""); 
  tft.println(""); 
  tft.setTextColor(TFT_WHITE,TFT_BLUE);  tft.setTextSize(1);
  tft.setTextFont(2);
  tft.print("Subscriptions = "); tft.println(subCt); 
  // for (int i = 0; i < 1000; i++) {
  //   yield(); tft.drawNumber(i, 0, 0, 7);
  // }
}

void setup()
{
  // Setup TFT
  tft.init();
  tft.setCursor(0, 0);
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.println("TFT Test String");
  tft.println("Its small so including another line.");
  #ifndef TOUCH_CS
    touch.init();
    touch.setMeasure(0, false, true, false, 2); // z first, take 2'th z,x,y
  #endif
};

void loop()
{
  if (touch.getXY(x, y)) {
    Serial.println("Touched");
  }
};