const int touchPin = 5;

void setup() {
  Serial.begin(115200);
  pinMode(touchPin, INPUT);
}

void loop() {
  int state = digitalRead(touchPin); // 1 (HIGH) = Touched
  Serial.println(state);
  delay(500);
}
