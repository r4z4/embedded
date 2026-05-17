int sensorPin = 2; // Connected to digital output of KY-038 sound sensor module
int ledPin = 13; // Connected to postive of led
boolean isOn = false; //To determine/track if led is on or off

void setup() {
  pinMode(sensorPin, INPUT); //Setting the pin to input for reading data
  pinMode(ledPin, OUTPUT); //Setting the pin to output for turning the led on/off

}

void loop() {

  int data = digitalRead(sensorPin); //Reading data from sensor and storing in variable

  if (data != 1) { // 1 is sent from sensor when loud noise is detected
    if (isOn == true) { // If led is on then turn it off
      digitalWrite(ledPin, LOW);
      isOn = false;
    }
    else { // else if led is off then turn it on
      digitalWrite(ledPin, HIGH);
      isOn = true;
    }
  }
}