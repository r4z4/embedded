#include <FastLED.h>   //Download & Install FastLED.h Library

//Pin Defination for Arduino - Uncomment below 2 lines if you are using Arduino and comment the below ATtiny85 Lines
// #define LEDPin 3       //WS2812b LED Strip  
// #define TouchPin 2     //Touch Sensor

//Pin Defination for ATtiny85 - Uncomment below 2 lines if you are using ATtiny85 and comment the above Arduino Lines
#define LEDPin 1       //WS2812b LED Strip  
#define TouchPin 0     //Touch Sensor

#define NUM_LEDS 7
CRGB leds[NUM_LEDS];

void setup() 
{
FastLED.addLeds<WS2812, LEDPin, GRB>(leds, NUM_LEDS);
FastLED.clear();
pinMode(TouchPin, INPUT_PULLUP);
StartPattern();
}

void loop() 
{
  for (int i = 0; i < 6; i++) 
  {
    diceNumber(i);
    delay(30);
    if (digitalRead(TouchPin) == HIGH) 
    {
      int x = 30;
      for (int j = 0; j < 15; j++) 
      {
        i++;
        if (i == 6) 
          {
            i = 0;
          }
        delay(x);
      leds[0] = CRGB(0, 0, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 0, 0);
      FastLED.show();
        delay(x);
        x += 5;
        diceNumber(i);
      }
      while (digitalRead(TouchPin) == LOW) {}
      for (int k = 0; k < 3; k++) 
      {
        delay(100);
      }
    }
      leds[0] = CRGB(0, 0, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 0, 0);
      FastLED.show();
      delay(30);
  }
}
void diceNumber(int n) 
{
  switch (n) 
  {
    case 0:                                 //Dice Roll 1
      leds[0] = CRGB(0, 0, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(255, 165, 155);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 0, 0);
      FastLED.show();
      break;
    case 1:                                 //Dice Roll 2
      leds[0] = CRGB(255, 255, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(255, 255, 0);
      FastLED.show();
      break;
    case 2:                                 //Dice Roll 3
      leds[0] = CRGB(0, 255, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 255, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 255, 0);
      FastLED.show();
      break;
    case 3:                                 //Dice Roll 4
      leds[0] = CRGB(0, 200, 255);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 200, 255);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 200, 255);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 200, 255);
      FastLED.show();
      break;
    case 4:                                 //Dice Roll 5
      leds[0] = CRGB(100, 0, 255);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(100, 0, 255);
      leds[3] = CRGB(100, 0, 255);
      leds[4] = CRGB(100, 0, 255);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(100, 0, 255);
      FastLED.show();
      break;
    case 5:                                 //Dice Roll 6
      leds[0] = CRGB(255, 0, 150);
      leds[1] = CRGB(255, 0, 150);
      leds[2] = CRGB(255, 0, 150);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(255, 0, 150);
      leds[5] = CRGB(255, 0, 150);
      leds[6] = CRGB(255, 0, 150);
      FastLED.show();
      break;
  }
}

void StartPattern ()
{
      leds[0] = CRGB(0, 0, 0);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(255, 100, 100);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(0, 0, 0);
      FastLED.show();
      delay(500);

      
      leds[0] = CRGB(255, 255, 20);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(255, 255, 20);
      FastLED.show();
      delay(500);

      leds[0] = CRGB(100, 255, 100);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(0, 0, 0);
      leds[3] = CRGB(100, 255, 100);
      leds[4] = CRGB(0, 0, 0);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(100, 255, 100);
      FastLED.show();
      delay(500);

      leds[0] = CRGB(20, 255, 255);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(20, 255, 255);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(20, 255, 255);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(20, 255, 255);
      FastLED.show();
      delay(500);

      leds[0] = CRGB(150, 50, 255);
      leds[1] = CRGB(0, 0, 0);
      leds[2] = CRGB(150, 50, 255);
      leds[3] = CRGB(150, 50, 255);
      leds[4] = CRGB(150, 50, 255);
      leds[5] = CRGB(0, 0, 0);
      leds[6] = CRGB(150, 50, 255);
      FastLED.show();
      delay(500);

      leds[0] = CRGB(255, 20, 120);
      leds[1] = CRGB(255, 20, 120);
      leds[2] = CRGB(255, 20, 120);
      leds[3] = CRGB(0, 0, 0);
      leds[4] = CRGB(255, 20, 120);
      leds[5] = CRGB(255, 20, 120);
      leds[6] = CRGB(255, 20, 120);
      FastLED.show();
      delay(500);
}      
