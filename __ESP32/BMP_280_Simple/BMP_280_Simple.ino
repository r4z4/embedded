#include <Wire.h>
#include  <Adafruit_BMP280.h>

Adafruit_BMP280  bmp;

void setup() {
  Serial.begin(115200);
  delay(1000);
  bmp.begin(0x76); // I2C Address. BMP has two. Will get NAN if incorrect.
  delay(2000);
}

void loop() {
  handleSensor1Touch();
  delay(2000);
}

void handleSensor1Touch() {
  // BMP280
  float rawTemp = bmp.readTemperature();
  Serial.print("Raw Temp: ");
  Serial.println(rawTemp);
  float T = bmp.readTemperature()*9/5  + 32; // Read temperature in C and conversion to F
  Serial.print("Temp: ");
  Serial.println(T);
  float rawPres = bmp.readPressure();
  Serial.print("Raw Pres: ");
  Serial.println(rawPres);
  float P = bmp.readPressure()/3386.39; //Read Pressure in Pa and conversion to inches of mercury
  Serial.print("Pres: ");
  Serial.println(P);
  float rawAlt = bmp.readAltitude();
  Serial.print("Raw Alt: ");
  Serial.println(rawAlt);
  float A  = bmp.readAltitude(1019.66)*3.28084; //Calculating the Altitude, the "1019.66"  is the pressure in(hPa) at sea level at day in your region
  Serial.print("Alt: ");
  Serial.println(A);
 
}




