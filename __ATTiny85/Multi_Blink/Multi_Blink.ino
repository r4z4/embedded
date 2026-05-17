const int btnPin = 1;
const int blueLed = 2;
const int greenLed = 3;
const int yellowLed = 4;

void setup() {
  pinMode(btnPin, INPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
}

void loop() {
  if (digitalRead(btnPin) == 1) {
    digitalWrite(blueLed, HIGH);
    digitalWrite(greenLed, HIGH); 
    digitalWrite(yellowLed, HIGH);
  } else {                  
    digitalWrite(blueLed, LOW); 
    digitalWrite(greenLed, LOW); 
    digitalWrite(yellowLed, LOW);  
  }
  delay(1000);                      
}
