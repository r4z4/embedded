// Define Pins for RGB LED (Must support PWM ~)
const int redPin = 21;
const int greenPin = 20;
const int bluePin = 10;

// Define Pins for Joystick
const int joyXPin = 1;
const int joyYPin = 0;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600); // Optional, for debugging
}

void loop() {
  // Read analog values from joystick (0 to 1023)
  int xValue = analogRead(joyXPin);
  int yValue = analogRead(joyYPin);

  // Map joystick values (0-1023) to LED PWM brightness (0-255)
  int redValue = map(xValue, 0, 1023, 0, 255);
  int blueValue = map(yValue, 0, 1023, 0, 255);
  
  // Create a mixed value for green (e.g., using both X and Y axes)
  int greenValue = map((xValue + yValue) / 2, 0, 1023, 0, 255);

  // Write values to the RGB LED
  analogWrite(redPin, redValue);
  analogWrite(greenPin, greenValue);
  analogWrite(bluePin, blueValue);

  // Print values to the Serial Monitor
  Serial.print("X: "); Serial.print(xValue);
  Serial.print(" | Y: "); Serial.println(yValue);
  
  delay(50); // Small delay for stability
}