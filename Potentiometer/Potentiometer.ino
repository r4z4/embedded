void setup() 
{
  Serial.begin(9600); // Lets Arduino print using Serial Monitor

}

void loop() 
{
  int sensorValue = analogRead(A0);  // Read input on Analog A0
  float voltage = sensorValue * (5.0 / 1023.0);
  int brightness = sensorValue/4; // 1023/4 = 255.75 (.75 goes away since its an int)
  analogWrite(11, brightness); // Send brightness value to LED pin, which we set on 11
  Serial.print("ADC Reading: ");
  Serial.print(sensorValue);
  Serial.print(" Analog voltage: ")
  Serial.println(voltage);
  delay(1);
}
