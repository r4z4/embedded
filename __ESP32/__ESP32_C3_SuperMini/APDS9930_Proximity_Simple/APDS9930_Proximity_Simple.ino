#include "Wire.h"
#include "APDS9930.h"

APDS9930 sensor = APDS9930();

void setup() {
  Serial.begin(9600);
  sensor.init();
  sensor.enableProximitySensor(false); // false = not using interrupt. constant reading.
  delay(500); 
}

void loop() {
  static uint16_t proximity = 0;
  
  if (sensor.readProximity(proximity)) {
    Serial.print("proximity:");
    Serial.println(proximity);
  }

  delay(500);
}