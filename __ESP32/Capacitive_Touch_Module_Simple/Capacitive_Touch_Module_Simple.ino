#define TOUCH_PIN 34

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32 Touch Test");
  pinMode(TOUCH_PIN, INPUT);
}

void loop() {
  Serial.println(digitalRead(TOUCH_PIN));
  delay(1000);
}