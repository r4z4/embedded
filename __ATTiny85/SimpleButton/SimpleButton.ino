const int buttonPin = 0; // Digitally controlled pin 0 (e.g., PB0)
const int ledPin = 1;    // Digitally controlled pin 1 (e.g., PB1)

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  // When using INPUT_PULLUP, a LOW reading means the button is pressed
  int buttonState = digitalRead(buttonPin);
  // Check if the button is pressed (buttonState is LOW)
  if (buttonState == LOW) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}