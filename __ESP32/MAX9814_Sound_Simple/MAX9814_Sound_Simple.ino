#define SOUND_PIN 35

void setup() {
  pinMode(SOUND_PIN, INPUT);
  Serial.begin(115200);
}

void loop() {
  int voice = analogRead(SOUND_PIN);
  Serial.println(voice);
  delay(100);
}
