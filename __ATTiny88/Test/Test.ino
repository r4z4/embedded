#define onboardLED 2

void setup() {
  pinMode(onboardLED, OUTPUT);
}

void loop() {
  digitalWrite(onboardLED, HIGH);
  delay(1000);
  digitalWrite(onboardLED, LOW);
  delay(1000);
}
