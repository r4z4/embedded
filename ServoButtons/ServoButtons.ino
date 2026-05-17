#include <Servo.h> // Servo library

int buttonPin = 4;
int button2Pin = 7;

int servoPin = 3;
Servo Servo1; // Servo object named Servo1
int servoPosition = 0; // Initial Servo position

void setup() 
{
  Servo1.attach(servoPin);
  Servo1.write(servoPosition);
  // Set up Serial Monitor on baud 9600 to display and receive data if needed
  Serial.begin(9600);

}

void loop() 
{
// Create vars to temporarily store the state of the buttons
  int buttonState = digitalRead(buttonPin); // High or Low depending on pressed or not
  int button2State = digitalRead(button2Pin);

  if (buttonState == HIGH && servoPosition < 180)// Button 1 is on
  {
    Servo1.write(servoPosition); // Write current positions
    servoPosition++; // Add 1 to position
    delay(5); // Allow it time to move
  }
  else if (button2State == HIGH && servoPosition > 0) // Button 2 is on
  {
    Servo1.write(servoPosition); // Write current positions
    servoPosition--; // Add 1 to position
    delay(5); // Allow it time to move
  }
}
