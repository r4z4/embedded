// C++ Code

void setup() 
{
  // pinMode(2, OUTPUT);
  DDR2 |= (1<<PD2);
}

void loop() 
{
  digitalWrite(2, HIGH);
  delay(2000); // Wait for 1000ms
  digitalWrite(2, LOW);
  delay(1000); // Wait for 1000ms
}
