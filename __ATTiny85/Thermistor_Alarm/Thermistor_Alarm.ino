
// Thermistor Alarm for ATTiny85
// Thermistor Pin: 2 (Physical Pin 7, ADC1)
// LED Pin: 0 (Physical Pin 5)

const int THERMISTOR_PIN = A1; // Pin 2 on ATTiny85 corresponds to Analog Input 1 (ADC1)
const int LED_YELLOW_PIN = 1;         // Pin 1
const int LED_PIN = 0;         // Pin 0

// Thermistor parameters (Assuming NTC 10k beta 3950 and 10k series resistor)
const float SERIES_RESISTOR = 10000.0;
const float NOMINAL_RESISTANCE = 10000.0;
const float NOMINAL_TEMPERATURE = 25.0;
const float B_COEFFICIENT = 3950.0;

float startingTemp = 0.0;

void setup() {
  pinMode(LED_PIN, OUTPUT);
  
  // Take a few readings to settle
  for(int i=0; i<5; i++) {
    getTemperature();
    delay(100);
  }
  
  // Measure starting temperature
  startingTemp = getTemperature();
  
  // Flash LED to indicate setup done
  digitalWrite(LED_PIN, HIGH);
  delay(500);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
  float currentTemp = getTemperature();
  
  // Check if temperature has risen more than 2 degrees
  if (currentTemp > (startingTemp + 0.5)) {
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_YELLOW_PIN, LOW);
  } else {
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_YELLOW_PIN, HIGH);
  }
  
  delay(1000); // Check every second
  digitalWrite(LED_YELLOW_PIN, LOW);
  delay(1000);
}

float getTemperature() {
  int analogValue = analogRead(THERMISTOR_PIN);
  
  // Prevent division by zero if reading is 1023 (open circuit/VCC)
  if (analogValue == 1023) return -273.15; 

  // Convert analog reading to resistance
  float resistance = SERIES_RESISTOR / (1023.0 / analogValue - 1.0);
  
  // Steinhart-Hart equation (Beta parameter version)
  float steinhart;
  steinhart = resistance / NOMINAL_RESISTANCE;        // (R/Ro)
  steinhart = log(steinhart);                         // ln(R/Ro)
  steinhart /= B_COEFFICIENT;                         // 1/B * ln(R/Ro)
  steinhart += 1.0 / (NOMINAL_TEMPERATURE + 273.15);  // + (1/To)
  steinhart = 1.0 / steinhart;                        // Invert
  steinhart -= 273.15;                                // convert to C
  
  return steinhart;
}
