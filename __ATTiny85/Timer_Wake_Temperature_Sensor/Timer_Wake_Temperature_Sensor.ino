/* 
   M. Ray Burnette 20140210 Open Source: for "Hot Yet?" publication
   Coding Badly Tiny core: https://code.google.com/p/arduino-tiny/
   Binary sketch size: 2,852 bytes (of a 8,192 byte maximum)
   Arduino 1.0.5 No bootloader used.  Upload via ISP.
   Project 3.3V Attiny85 @8MHz internal,  under 10mA idle at 68F

   Schematic:
   x-----------------/\/\/\/\/\/\/\/---------------x---/\/\/\/\/\/\/\----xGND
   |                NTC 10K Thermistor             |  10K 1% Resistor
   |                                               |
   |              ATTINY85 / ARDUINO               |
   |                     +-\/-+                    |
   |    Ain0 (D 5) PB5  1|    |8  Vcc              |
   x--- Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1 --- 
(Blue)- Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1  (Green) ----------|<---x
   |               GND  4|    |5  PB0 (D 0) pwm0  (Red) ------------|<---x
   |                     +----+                                          |    100 Ohm
   x----------------------------------------------------------------|<---x---/\/\/\/\/---Vcc 3.3V

*/
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>

boolean flag_wdt = 1;

int pinT = PB3;           // Thermistor source voltage
int pinR = PB0;           // Digital pin #0  Red
int pinG = PB1;           // Digital pin #1  Green
int pinB = PB4;           // Digital pin #4  Blue
int r; int g; int b;
const int nToSleep = 50 ; // # of stable temp readings before sleep
const int Delay =    100; // main loop delay in mS
double ADCcount;
double ADCprevious;
int nCount;
int ThermistorPin  = 1 ;  // A1 is physical pin #7 (PB2)

void setup()
{
  // WDTO_15MS, WDTO_30MS, WDTO_60MS, WDTO_120MS, WDTO_250MS, WDTO_500MS, 
  // WDTO_1S, WDTO_2S, WDTO_4S, WDTO_8S
  setup_watchdog(WDTO_4S);  // Periodic Heartbeat to awaken deep sleep()
  sleep_disable();
  pinMode(pinT, OUTPUT); digitalWrite(pinT, HIGH);  // Thermistor Source
  pinMode(pinR, OUTPUT);
  pinMode(pinG, OUTPUT);
  pinMode(pinB, OUTPUT);
}
  
void loop() 
{
  wdt_reset();    // pat K9
  ADCcount = analogRead(ThermistorPin) ;
  if (ADCcount == ADCprevious) ++nCount;
  if ( nCount > nToSleep )
  { // prepare for low current power-down state
    pinMode(pinR, INPUT); digitalWrite(pinR, HIGH);  // pullup enabled
    pinMode(pinG, INPUT); digitalWrite(pinG, HIGH);
    pinMode(pinB, INPUT); digitalWrite(pinB, HIGH);
    SleepLonger:    // Come here to re-sleep
    pinMode(pinT, INPUT); digitalWrite(pinT, HIGH);
      system_sleep();
      sleep_disable();  // deep sleep until WDT kicks
      pinMode(pinT, OUTPUT); digitalWrite(pinT, HIGH);
      delay(50);
      // Yawn, exercise a few reads for stabilization
      for (uint8_t z=0; z<5; z++) {
        ADCcount = analogRead(ThermistorPin) ;
      }
      if (abs(ADCcount - ADCprevious) < 4) goto SleepLonger;  // hysteresis
    // restore LED output drivers ... temp has gone up
    pinMode(pinR, OUTPUT); digitalWrite(pinR, HIGH);
    pinMode(pinG, OUTPUT); digitalWrite(pinG, HIGH);
    pinMode(pinB, OUTPUT); digitalWrite(pinB, HIGH);
    n