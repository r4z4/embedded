#include "LedController.hpp"
/*
 You might need to change the following 3 Variables depending on your board.
 pin 20 is connected to the DataIn
 pin 4 is connected to the CLK
 pin 21 is connected to LOAD/ChipSelect
*/
#define DIN 20
#define CS 21
#define CLK 4

// This works but in a strange way. It appears as:
// H 51|t 74
// 76543210
// Was expecting 74 T|51 H
// Iterating over integer gets last digit first
// To reverse and have it be more intuitive, convert into char
// and iterate over the char instead

int temp_digit;
int temp_copy;
int temp_position = 0;
int hum_digit;
int hum_copy;
int hum_position = 4; // Second part of display

/*
 Now we need a LedController Variable to work with.
 We have only a single MAX72XX so the Dimensions are 1,1.
 */
LedController<1,1> lc;

/* we always wait a bit between updates of the display */
unsigned long delaytime=1000;

void setup() {
  //Here a new LedController object is created without hardware SPI.
  lc=LedController<1,1>(DIN,CLK,CS);
  lc.setIntensity(8); /* Set the brightness to a medium values */
  lc.clearMatrix(); /* and clear the display */
}

void setTemp(int temp) {
  temp_copy = temp;
  while (temp_copy > 0) {
    temp_digit = temp_copy % 10; // Get last digit
    lc.setDigit(0, temp_position, temp_digit, false);
    temp_position++;
    temp_copy /= 10; // Remove last digit
  }
  lc.setChar(0, temp_position, ' ', false); // Insert blank space
  lc.setChar(0, temp_position + 1, 'T', false);
}

void setHum(int hum) {
  hum_copy = hum;
  while (hum_copy > 0) {
    hum_digit = hum_copy % 10; // Get last digit
    lc.setDigit(0, hum_position, hum_digit, false);
    hum_position++;
    hum_copy /= 10; // Remove last digit
  }
  lc.setChar(0, hum_position, ' ', false); // Insert blank space
  lc.setChar(0, hum_position + 1, 'H', false);
}

void exampleDisplay() {
  for(int i=0; i<8; i++) {
    lc.setDigit(0,i,i,false);
    delay(delaytime);
  }
  lc.clearMatrix();
  delay(delaytime);
}


void loop() {
  setTemp(74);
  setHum(51);
  delay(1000);
  lc.clearMatrix();
  // Reset positions
  temp_position = 0;
  hum_position = 4;
}