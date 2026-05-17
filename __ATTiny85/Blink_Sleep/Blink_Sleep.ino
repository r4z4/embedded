#include <avr/sleep.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BY(bit))
#endif

int pinLed = 0;

void setup() {
    pinMode(pinLed, OUTPUT);
    pinMode(1, INPUT);

    sbi(GIMSK, PCIE);   // Turn on Pin Change Interrupt
    sbi(PCMSK, PCINT0); // Which pins affected by interrupt
}

void loop() {
    digitalWrite(pinLed, HIGH);
    delay(1000);
    digitalWrite(pinLed, LOW);
    system_sleep();
}

void system_sleep() {
    cbi(ADCSRA, ADEN);                      // Switch Analog to Digital Converter OFF
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);    // Set sleep mode
    sleep_mode();                           // System sleeps here
    sbi(ADCSRA, ADEN);                      // Switch Analog to Digital Converter ON
}

ISR(PCINT0_vect) {
}


