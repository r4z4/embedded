#include <Servo.h> // Servo library (Will always assume Servo max is 180)

int servoPin = 3;
Servo Servo1; // Servo object named Servo1
int servoPosition = 0; // Initial Servo position
int actualAngle;

void setup() 
{
  Servo1.attach(servoPin);
  Servo1.write(servoPosition);
  // Set up Serial Monitor on baud 9600 to display and receive data if needed
  Serial.begin(9600);
}

void loop() 
{
  for (actualAngle = 0; actualAngle <= 160; actualAngle += 1) {
    servoPosition = map(actualAngle, 0, 160, 0, 180);
    Servo1.write(servoPosition);
    delay(30);
  }
  for (servoPosition = 180; servoPosition >= 0; servoPosition -= 1) {
    Servo1.write(servoPosition);
    delay(30);
  }
}