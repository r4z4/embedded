#include "TinyDebug.h"

// The 74HC595 uses a type of serial connection called SPI
// (Serial Peripheral Interface) that requires three pins:

#define DATA_PIN  PB0 // Pin connected to DS of 74HC595
#define CLOCK_PIN PB2 // Pin connected to SHCP of 74HC595
#define LATCH_PIN PB1 // Pin connected to STCP of 74HC595

// Global variable for the data we're sending to the shift register
uint8_t data = 0;
uint8_t shift = 0;
uint8_t lastShift = 0;
uint16_t delayTime = 0;
uint16_t lastDelay = 0;

void setup() {
  Debug.begin();
  delay(100);
  // Set pins to potentiometers input
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  // Set pins to 74HC595 output
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
}

void loop() {
  // Change to try the different functions below
  shift = map(analogRead(A2), 0, 1023, 1, 6);

  // Make this smaller for faster switching
  delayTime = map(analogRead(A3), 0, 1023, 25, 125);

  if (shift != lastShift) {
    lastShift = shift;
    Debug.print("Function: ");
    Debug.println(shift);
  }

  if (delayTime != lastDelay) {
    lastDelay = delayTime;
    Debug.print("Delay(ms) ");
    Debug.println(delayTime);
  }

  switch (shift) {
    case 1: oneAfterAnother(); // All on, all off
      break;
    case 2: oneOnAtATime();    // Scroll down the line
      break;
    case 3: pingPong();        // Like above, but back and forth
      break;
    case 4: randomLED();       // Blink random LEDs
      break;
    case 5: marquee();
      break;
    case 6: binaryCount();     // Bit patterns from 0 to 255
      break;
  }
}

void shiftWrite(int desiredPin, boolean desiredState) {
  // This function lets you make the shift register outputs
  // HIGH or LOW in exactly the same way that you use digitalWrite()
  bitWrite(data, desiredPin, desiredState); // Change desired bit to 0 or 1 in "data"

  // Now we'll actually send that data to the shift register.
  // The shiftOut() function does all the hard work of
  // manipulating the data and clock pins to move the data
  // into the shift register:
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data); // Send "data" to the shift register

  // Toggle the latchPin to make "data" appear at the outputs
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(LATCH_PIN, LOW);
}

void oneAfterAnother() {
  // This function will turn on all the LEDs, one-by-one,
  // and then turn them off all off, one-by-one
  int index;

  // Turn all the LEDs on
  for (index = 0; index <= 7; index++) {
    shiftWrite(index, HIGH);
    delay(delayTime);
  }

  // Turn all the LEDs off
  for (index = 7; index >= 0; index--) {
    shiftWrite(index, LOW);
    delay(delayTime);
  }
}

void oneOnAtATime() {
  // This function will turn the LEDs on and off, one-by-one
  int index;

  // Step through the LEDs, from 0 to 7
  for (index = 0; index <= 7; index++) {
    shiftWrite(index, HIGH); // Turn LED on
    delay(delayTime);        // Pause to slow down the sequence
    shiftWrite(index, LOW);  // Turn LED off
  }
}

void pingPong() {
  // This function turns on the LEDs, one at a time, in both directions
  int index;

  // Step through the LEDs, from 0 to 7
  for (index = 0; index <= 7; index++) {
    shiftWrite(index, HIGH); // Turn LED on
    delay(delayTime);        // Pause to slow down the sequence
    shiftWrite(index, LOW);  // Turn LED off
  }

  // Step through the LEDs, from 7 to 0
  for (index = 7; index >= 0; index--) {
    shiftWrite(index, HIGH); // Turn LED on
    delay(delayTime);        // Pause to slow down the sequence
    shiftWrite(index, LOW);  // Turn LED off
  }
}

void randomLED() {
  // This function will randomly turn on and off LEDs
  int index;

  index = random(8);         // Pick a random number between 0 and 7

  shiftWrite(index, HIGH);   // Turn LED on
  delay(delayTime);          // Pause to slow down the sequence
  shiftWrite(index, LOW);    // Turn LED off
}

void marquee() {
  // This function will mimic "chase lights" like those around signs
  int index;

  // Step through the first four LEDs
  // (We'll light up one in the lower 4 and one in the upper 4)
  for (index = 0; index <= 3; index++) {
    shiftWrite(index, HIGH);     // Turn a LED on
    shiftWrite(index + 4, HIGH); // Skip four, and turn that LED on
    delay(delayTime);            // Pause to slow down the sequence
    shiftWrite(index, LOW);      // Turn both LEDs off
    shiftWrite(index + 4, LOW);
  }
}

void binaryCount() {
  // This function creates a visual representation of the
  // on/off pattern of bits in a byte

  // Send the data byte to the shift register:
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, data);

  // Toggle the latch pin to make the data appear at the outputs:
  digitalWrite(LATCH_PIN, HIGH);
  digitalWrite(LATCH_PIN, LOW);

  // Add one to data, and repeat!
  // (Because a byte type can only store numbers from 0 to 255,
  // if we add more than that, it will "roll around" back to 0
  // and start over)
  data++;

  // Delay so you can see what's going on:
  delay(delayTime);
}
