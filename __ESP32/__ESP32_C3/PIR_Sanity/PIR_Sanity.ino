const int pir = 3;
int motion;

void setup() {
  Serial.begin(9600);
  pinMode(pir, INPUT);
}

void loop() {
  motion = digitalRead(pir);
  Serial.print("Motion Reading: ");
  Serial.print(motion);
  Serial.println("");
  // if (motion) {
  //   digitalWrite(led, HIGH);
  // } else {
  //   digitalWrite(led, LOW);
  // }
  delay(1000);
}