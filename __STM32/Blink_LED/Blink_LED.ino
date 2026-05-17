const byte LEDOnState = LOW;
const byte LEDOffState = HIGH;

void setup() {}

void loop() {
  digitalWrite(LED_BUILTIN, LEDOnState);    // turn the LED on
  delay(1000);                              // wait for a second
  digitalWrite(LED_BUILTIN, LEDOffState);   // turn the LED off
  delay(10000);                             // wait for a second
}