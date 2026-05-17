// Gpios of ESP32 work at 3.3 volts. If get reading of 4095 this is the likely issue.

#define sensorPin 4

// pinMode(A0,INPUT); // Does work w/ ESP just make sure right pin on board

// // Simple E.g.

// void setup() {
//   pinMode(36, INPUT);
//   Serial.begin(9600);
// }

// void loop() {
//   float pulse;
//   int sum = 0;
//   for (int i = 0; i < 20; i++)
//     sum += analogRead(sensorPin);
//   pulse = sum / 20.00;
//   Serial.println(pulse);
//   delay(100);
// }


double alpha=0.75;
int period=20;
double refresh=0.0;

void setup(void)
{
  Serial.begin(9600);
  pinMode(sensorPin, INPUT);
}

void loop(void)
{
  static double oldValue=0;
  static double oldrefresh=0;
  int beat=analogRead(sensorPin);
  double value=alpha*oldValue+(0-alpha)*beat;
  refresh=value-oldValue;
  Serial.print(" Heart Monitor "); 
  Serial.print("          ");
  Serial.println(beat/10);
  oldValue=value;
  oldrefresh=refresh;
  delay(period*10);
}