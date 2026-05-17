#include <Wire.h>
#include <Adafruit_Sensor.h> 
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht20;
sensors_event_t aht20Temp, aht20Hum;
const String SENSOR_NAME = "AHT20 T & H";  // don't extend the string with more than 14 characters !
float temperature = -99;
float humidity = -99;

void getAht20Values() {
  aht20.getEvent(&aht20Hum, &aht20Temp);// populate temp and humidity objects with fresh data
  temperature = aht20Temp.temperature;
  humidity = aht20Hum.relative_humidity;
}

void setup() {
  Serial.begin(115200);
}

void loop() {
  getAht20Values();
}
