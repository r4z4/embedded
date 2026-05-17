Key changes made:
┌─────────────────────────────────────┬──────────────┬─────────────────────────────────────────────────────────┐
│ ESP32 C3 │ ATTiny85 │ Reason │
├─────────────────────────────────────┼──────────────┼─────────────────────────────────────────────────────────┤
│ IRAM_ATTR │ removed │ ESP32-specific, not needed on AVR │
├─────────────────────────────────────┼──────────────┼─────────────────────────────────────────────────────────┤
│ Serial.begin() / Serial.println() │ removed │ No hardware UART on ATTiny85 │
├─────────────────────────────────────┼──────────────┼─────────────────────────────────────────────────────────┤
│ digitalPinToInterrupt(motionSensor) │ 0 │ ATTiny85 uses interrupt number directly; pin 2 = INT0 │
├─────────────────────────────────────┼──────────────┼─────────────────────────────────────────────────────────┤
│ LED on pin 1 │ LED on pin 0 │ Pin 1 works too, but keeping pin 2 free for INT0 │
├─────────────────────────────────────┼──────────────┼─────────────────────────────────────────────────────────┤
│ PIR on pin 4 │ PIR on pin 2 │ Pin 2 (PB2) is the only hardware external interrupt pin │
└─────────────────────────────────────┴──────────────┴─────────────────────────────────────────────────────────┘
ATTiny85 pinout reference:
ATTiny85
+--U--+
RST 1| |8 VCC
PB3 2| |7 PB2 (INT0) <- PIR Sensor
PB4 3| |6 PB1
GND 4| |5 PB0 <- LED
+-----+

Notes:

- You'll need ATTinyCore or a similar core installed in Arduino IDE
- If you need debugging, consider using SoftwareSerial on pins 3/4, or blink patterns to indicate state
- The timeSeconds variable isn't used in your original code - if you want a timeout feature, let me know and I can add
  it
