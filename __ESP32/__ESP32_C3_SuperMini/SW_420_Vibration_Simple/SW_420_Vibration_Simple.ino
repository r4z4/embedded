const int sensorPin = 20;

void setup() {
  Serial.begin(115200);         
  pinMode(sensorPin, INPUT);  
}

void loop() {
  if (digitalRead(sensorPin)) {              
    Serial.println("Detected vibration...");  
  } 
  else {
    Serial.println("...");  
  }
  delay(100);
}