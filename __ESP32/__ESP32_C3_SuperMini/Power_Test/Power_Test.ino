// the setup function runs once when you press reset or power the board
#define LED_PIN 4

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(LED_PIN, HIGH);  
  delay(3000);  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);  
  delay(2000);  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
  digitalWrite(LED_PIN, HIGH);  
  delay(1000);  
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}
