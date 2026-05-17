const int piezoPin = 36; // Pin connected to the analog sensor (S)
const int ledPin = 26; 
const int threshold = 50; // Sensitivity threshold (adjust as needed 0-1023)

int sensorReading = 0;   // Variable to hold sensor value

void setup() {
  pinMode(ledPin, OUTPUT);
  Serial.begin(115200); // Start serial monitoring
}

void loop() {
  // Read the piezo sensor
  sensorReading = analogRead(piezoPin);

  // If reading is higher than threshold, a knock/vibration is detected
  if (sensorReading > threshold) {
    Serial.print("Vibration Detected: ");
    Serial.println(sensorReading);
    
    // Turn LED on to visualize
    digitalWrite(ledPin, HIGH);
    delay(100); // Small delay to visualize
    digitalWrite(ledPin, LOW);
  }
  
  // Optional: Add a small delay for stability, but remove for high speed detection
  // delay(1);
}