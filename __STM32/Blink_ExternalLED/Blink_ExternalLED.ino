// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.print("Hey Ref");
  Serial1.print("Hey 1");
  pinMode(PA5, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(PA5, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(PA5, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
