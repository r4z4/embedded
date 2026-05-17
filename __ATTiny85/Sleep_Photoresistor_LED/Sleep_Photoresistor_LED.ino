#include <avr/interrupt.h>
#include <avr/sleep.h>

int ledPin = 4;            // LED connected to digital pin 0
int interruptPin = 3;      // LED to show the action of a interrupt
// Can only trigger interrupt from sleep on LOW - due to clocks disabled
int wakePin = 2;           // active LOW, connect to GND to wake up


void setup(){
  pinMode(ledPin, OUTPUT);          // sets the digital pin as output
  pinMode(interruptPin, OUTPUT);    // sets the digital pin as output
  pinMode(wakePin, INPUT);          // sets the digital pin as input
  digitalWrite(wakePin, HIGH);
}


void sleepNow(){         
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // sleep mode is set here
  sleep_enable();                         // enables the sleep bit in the mcucr register so sleep is possible
  attachInterrupt(0, wakeUpNow, FALLING);     // use interrupt 0 (pin 2) and run function wakeUpNow on falling edge on pin 2
  digitalWrite(ledPin, LOW); 
    
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


void loop(){
  digitalWrite(ledPin, HIGH);      // sets the LED on
  delay(5000);                     // waits for a second
  sleepNow();                      // sleep function called here
}