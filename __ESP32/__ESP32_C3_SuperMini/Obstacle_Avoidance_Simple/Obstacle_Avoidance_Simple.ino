/*
  This code reads the digital value from a Infrared obstacle avoidance sensor connected to 
  pin 25 and prints it to the serial monitor every 50 milliseconds.
  When the module detects obstacles ahead, the red indicator light on the module will 
  illuminate and the OUT port will continuously output a low-level signal.
*/

// Define the pin numbers for the Infrared obstacle avoidance sensor
const int sensorPin = 25;

void setup() {
  pinMode(sensorPin, INPUT);
  Serial.begin(9600);
}

void loop() {
  Serial.println(digitalRead(sensorPin));
  delay(50);
}