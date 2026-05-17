const int IRPin = 2;
const int ledPin = 13;

int LDRState = 0;
void setup() {
  // Initialize LED Pin as Output
  pinMode(ledPin, OUTPUT);
  // IRPin as Input
  pinMode(IRPin, INPUT);

}

void loop() {
  LDRState = digitalRead(IRPin);
}