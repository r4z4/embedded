// #include <DigisparkOLED.h>
#include <TinyWireM.h>
#include <Tiny4kOLED.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "dht.h"

#define DHTPIN 4
dht DHT;

// int ledPin = 4;            // LED connected to digital pin 0
int interruptPin = 3;      // LED to show the action of a interrupt
int wakePin = 1;           // active LOW, ground this pin momentarily to wake up

void setup()
{
  // pinMode(ledPin, OUTPUT);          // sets the digital pin as output
  pinMode(interruptPin, OUTPUT);    // sets the digital pin as output
  pinMode(wakePin, INPUT);          // sets the digital pin as input
  digitalWrite(wakePin, HIGH);
  oled.begin();
  oled.clear();
  oled.setFont(FONT8X16);
  oled.on(); // Don't forget this
  triggerSensorResponse();
  sleepNow();
}

void loop(){}

void printToScreen(char* printStr, char* printStr2)
{   
  oled.setFont(FONT8X16);
  oled.setCursor(10, 0);
  oled.println(printStr);
  oled.println(printStr2);
  delay(2000);
}

void triggerSensorResponse() {
  int chk = DHT.read22(DHTPIN);
  if (chk == DHTLIB_OK) {
    float h = DHT.humidity;
    float t = DHT.temperature;

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(t)) {
      return;
    }

    char temp_message[10];
    char hum_message[10];
    snprintf(temp_message, sizeof(temp_message), "Temp: %.1f", t);
    snprintf(hum_message, sizeof(hum_message), "Hum: %.1f", h);
    printToScreen(temp_message, hum_message);
  }
}

void sleepNow(){         
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // sleep mode is set here
  sleep_enable();                         // enables the sleep bit in the mcucr register so sleep is possible
  attachInterrupt(0, wakeUpNow, LOW);     // use interrupt 0 (pin 2) and run function wakeUpNow when pin 2 gets LOW
  // digitalWrite(ledPin, LOW); 
  oled.off();
    
  sleep_mode();                           // here the device is actually put to sleep!!
    
  sleep_disable();                        // first thing after waking from sleep: disable sleep...
  detachInterrupt(0);                     // disables interrupton pin 3 so the wakeUpNow code will not be executed during normal running time.
  delay(250);                             // wait 2 sec. so humans can notice the interrupt LED to show the interrupt is handled
  digitalWrite (interruptPin, LOW);       // turn off the interrupt LED
}

void wakeUpNow(){        // here the interrupt is handled after wakeup
//execute code here after wake-up before returning to the loop() function
// timers and code using timers (serial.print and more...) will not work here.
  digitalWrite(interruptPin, HIGH);
}