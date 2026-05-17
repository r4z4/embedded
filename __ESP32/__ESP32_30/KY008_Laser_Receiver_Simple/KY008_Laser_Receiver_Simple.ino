const int receiverPin = 36; // Laser Receiver Output

void setup() {
  Serial.begin(115200);
  
  pinMode(receiverPin, INPUT);
}

void loop() {
  // Read the receiver state
  // HIGH usually means laser is detected, LOW means beam is broken
  int beamStatus = digitalRead(receiverPin);

  if (beamStatus == HIGH) {
    Serial.println("Beam secure.");
  }
  delay(100); // Small delay to prevent serial flooding
}