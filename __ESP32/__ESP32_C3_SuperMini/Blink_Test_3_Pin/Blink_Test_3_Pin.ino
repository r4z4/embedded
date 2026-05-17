// the setup function runs once when you press reset or power the board
#define G_LED_PIN 3
#define W_LED_PIN 1
#define B_LED_PIN 2

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  Serial.begin(115200);
  pinMode(G_LED_PIN, OUTPUT);
  pinMode(W_LED_PIN, OUTPUT);
  pinMode(B_LED_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  Serial.println("Setting Green LED");
  digitalWrite(G_LED_PIN, HIGH);  
  delay(3000);  
  digitalWrite(G_LED_PIN, LOW);
  delay(3000);
  
  Serial.println("Setting White LED");
  digitalWrite(W_LED_PIN, HIGH);
  delay(3000);
  digitalWrite(W_LED_PIN, LOW);
  delay(3000);

  // Serial.println("Setting Blue LED");
  // digitalWrite(B_LED_PIN, HIGH);
  // delay(3000);
  // digitalWrite(B_LED_PIN, LOW);
  // delay(3000);
}
