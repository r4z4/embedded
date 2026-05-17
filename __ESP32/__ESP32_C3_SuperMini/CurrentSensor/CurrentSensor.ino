#include <math.h> // Required for pow() function
#include "LedController.hpp"

#define DIN 20
#define CS 21
#define CLK 4

LedController<1,1> lc;
unsigned long delaytime=1000; // Always wait a bit between updates of display

const int adcPin = 0;
const float referenceVoltage = 3.3; // 3.3V
const int adcResolution = 12.0; // 12 bit ADC resolution. Arduino has 10.
const int possibleAdcValues = pow(2.0, adcResolution); // Can remove lib and just use 4095 here.
const float rangeMax = possibleAdcValues - 1.0;
const float currentSensorSensitivity = 0.100; // From datasheet. 100 mV per amp.

char charArray[8];
int length = 0;

void setup() {
  Serial.begin(9600);
  lc = LedController<1,1>(DIN,CLK,CS);
  lc.setIntensity(8); /* Set the brightness to a medium values */
  lc.clearMatrix(); /* and clear the display */
}
void loop() {
  int adc = analogRead(adcPin);
  float voltage = adc * referenceVoltage / rangeMax;
  // From datasheet. Voltage Output at 0A = 0.5 * Vcc. Our referenceVoltage is same as Vcc here.
  float current = (voltage - (referenceVoltage * 0.5)) / currentSensorSensitivity;
  if (current < 0.16) { // Ignoring small readings for handling noise
    current = 0;
  }
  Serial.print("Current : ");
  Serial.println(current);
  sprintf(charArray, "%.2f", current);
  length = strlen(charArray);
  for (int i = 0; i < length; i++) {
    lc.setChar(0, i, charArray[i], false);
  }
  delay(300);
}