const int led = 8;
const int pir = 2;
int motion;

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(pir, INPUT);
}

void loop() {
  motion = digitalRead(pir);
  Serial.print("Motion Reading: ");
  Serial.print(motion);
  Serial.println("");
  if (motion) {
    digitalWrite(led, HIGH);
  } else {
    digitalWrite(led, LOW);
  }
}
