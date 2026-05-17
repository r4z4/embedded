// Define the pin connected to the OUT pin of the HLK-LD1020
const int sensorPin = 35; 
int sensorState = 0;

void setup() {
  // Initialize serial communication
  Serial.begin(115200);
  // Set the sensor pin as input
  pinMode(sensorPin, INPUT);
  Serial.println("HLK-LD1020 Sensor Test Started");
}

void loop() {
  // Read the state of the sensor
  sensorState = digitalRead(sensorPin);
  
  // Check if motion is detected (output is high)
  if (sensorState == HIGH) {
    Serial.println("Motion Detected!");
  } else {
    Serial.println("No Motion");
  }
  
  delay(100); // Small delay for stability
}