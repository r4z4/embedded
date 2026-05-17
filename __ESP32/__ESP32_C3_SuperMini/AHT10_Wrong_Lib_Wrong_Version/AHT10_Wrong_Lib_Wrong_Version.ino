#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht10;
sensors_event_t aht10Temp, aht10Hum;
const String SENSOR_NAME = "AHT10 T & H";  // don't extend the string with more than 14 characters !
float temperature = -99;
float humidity = -99;

void getAht10Values() {
  aht10.getEvent(&aht10Hum, &aht10Temp);// populate temp and humidity objects with fresh data
  temperature = aht10Temp.temperature;
  humidity = aht10Hum.relative_humidity;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  getAht10Values();
}
