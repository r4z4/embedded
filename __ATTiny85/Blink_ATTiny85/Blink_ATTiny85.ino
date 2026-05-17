#include "tinysnore.h"
#define LED 4

int lightCount = 0;
int countMax = 10;

void setup() {
  pinMode(LED, OUTPUT);
}

void loop() {
  if (lightCount < countMax) {
    digitalWrite(LED, HIGH);  
    delay(1000);            
    digitalWrite(LED, LOW);   
    delay(1000);
    lightCount++;
  } else {
    lightCount = 0; // Reset 
    snore(10000);   // Deep sleeps for 10 seconds, then resumes from here
  }
}
