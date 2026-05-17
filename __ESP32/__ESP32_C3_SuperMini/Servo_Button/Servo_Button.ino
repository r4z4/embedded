#include <ESP32Servo.h>
// Struggles w/ 3.3V
//Servo

int pos = 0;    // Var to store the servo position
int servoPin = 7; // PWM Pins on Mini = 4-14
int btnPin = 1;

Servo servo1;

void unlock() {
  for(int posDegrees = 0; posDegrees <= 90; posDegrees++) {
    servo1.write(posDegrees);
    Serial.println(posDegrees);
    delay(20);
  }

  delay(2000);

  for(int posDegrees = 90; posDegrees >= 0; posDegrees--) {
    servo1.write(posDegrees);
    Serial.println(posDegrees);
    delay(20);
  }
}

void setup() {
  Serial.begin(115200);
  servo1.attach(servoPin);
  pinMode(btnPin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(btnPin) == LOW) {
    unlock();
  }
}