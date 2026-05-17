#define LED_PIN 13

void setup() 
{
  pinMode(LED_PIN, OUTPUT);
}

void loop() 
{
  digitalWrite(LED_PIN, HIGH);
  delay(2000); // Wait for 1000ms
  digitalWrite(LED_PIN, LOW);
  delay(1000); // Wait for 1000ms
}