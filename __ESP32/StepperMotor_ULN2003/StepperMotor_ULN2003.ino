#include <Stepper.h>

const int stepsPerRevolution = 2048;  // Change to fit # of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 19
#define IN2 18
#define IN3 5
#define IN4 17

Stepper myStepper(stepsPerRevolution, IN1, IN3, IN2, IN4); // Init stepper library

void setup() {
  myStepper.setSpeed(5); // Set speed at 5 rpm
  Serial.begin(115200);
}

void loop() {
  Serial.println("clockwise"); // Step one revolution in one direction
  myStepper.step(stepsPerRevolution);
  delay(1000);

  Serial.println("counterclockwise"); // Step one revolution in other direction
  myStepper.step(-stepsPerRevolution);
  delay(1000);
}