// Pin where the speaker/buzzer is connected
const int buzzerPin = 8;
const int buttonPin = 2;
const int delayTime = 400;

// Define the notes of the song (in Hz)
const int C4 = 261;
const int D4 = 294;
const int E4 = 329;
const int F4 = 349;
const int G4 = 392;
const int B4 = 466;
const int C5 = 523;

void setup() {
  // No setup needed for this example
  pinMode(buzzerPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  int buttonState = digitalRead(buttonPin);
  if (buttonState == LOW) {  // Pressed
    tone(buzzerPin, C4);
    delay(delayTime);
    tone(buzzerPin, D4);
    delay(delayTime);
    tone(buzzerPin, E4);
    delay(delayTime);
    tone(buzzerPin, F4);
    delay(delayTime);
    tone(buzzerPin, G4);
    delay(delayTime);
  } else {
    noTone(buzzerPin);
  }
}
