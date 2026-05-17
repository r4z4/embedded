void setup() 
{
  pinMode(12, OUTPUT);
  pinMode(2, INPUT);
}

void loop() {
  if(digitalRead(2) == HIGH) { // If Voltage read on pin 2 is HIGH
    digitalWrite(12, HIGH); // Button pressed, LED on
  }
  else {
    digitalWrite(12, LOW); // Button not pressed, LED off
  }
}
