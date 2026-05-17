#include "Adafruit_SHT4x.h"

Adafruit_SHT4x sht4x = Adafruit_SHT4x();

void setup() {
  Serial.begin(115200);
  if (!sht4x.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  // Set precision: SHT4X_HIGH_PRECISION, SHT4X_MED_PRECISION, SHT4X_LOW_PRECISION
  sht4x.setPrecision(SHT4X_HIGH_PRECISION);
  // Set heater: SHT4X_NO_HEATER, SHT4X_HEATER_1S, SHT4X_HEATER_100MS
  sht4x.setHeater(SHT4X_NO_HEATER);
}

void loop() {
  sensors_event_t humidity, temp;
  sht4x.getEvent(&humidity, &temp); // Populate temp and humidity objects with fresh data
  
  Serial.print("Temperature: "); Serial.print(temp.temperature); Serial.println(" *C");
  Serial.print("Humidity: "); Serial.print(humidity.relative_humidity); Serial.println(" %RH");
  
  delay(1000);
}