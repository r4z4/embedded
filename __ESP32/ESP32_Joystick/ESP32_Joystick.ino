int xPin = 26;
int yPin = 27;
int buttonPin = 13;

// Need var to  house values for each axis
int xVal;
int yVal;
int buttonState;

void setup() {
  Serial.begin(9600);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP); // Arduino has its own internal pullup resistor
}

void loop() {
  xVal = analogRead(xPin);
  yVal = analogRead(yPin);
  buttonState = digitalRead(buttonPin);

  Serial.print("\nX: ");
  Serial.print(xVal);  

  Serial.print(" | Y: ");
  Serial.print(yVal);  

  Serial.print(" | Btn: ");
  Serial.print(buttonState);  

  delay(1000);
}
