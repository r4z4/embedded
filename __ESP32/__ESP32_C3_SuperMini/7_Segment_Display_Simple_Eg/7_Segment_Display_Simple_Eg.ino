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

void setTemp(temp) {
  int digit;
  int temp_number = number;
  lc.setDigit
}


void loop() {
  for(int i=0; i<8; i++) {
    lc.setDigit(0,i,i,false);
    delay(delaytime);
  }
  lc.clearMatrix();
  delay(delaytime);
}