#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define MAX_ROUNDS 30
// LEDs
// uint8_t?
const int GREEN = 0;
const int RED = 2;
const int BLUE = 3;
const int YELLOW = 4;

const int T = 300000;

const int GREEN_BTN = PB3;
const int RED_BTN = PB4;

int greenState = 0;
int redState = 0;
int t = 0;
int pressed = 0;

int played_array[] = {};
int color_list[] = {1,2,3,4};
int list_size = sizeof(color_list) / sizeof(color_list[0]);

void setup() {
  srand(time(NULL)); // Seed the random number generator
  pinMode(GREEN_BTN, INPUT_PULLUP); // No pulldown on ATTiny
  pinMode(RED_BTN, INPUT_PULLUP);
}

int readButtons() {
  greenState = digitalRead(GREEN_BTN);
  redState = digitalRead(RED_BTN);
  if (greenState == 1) {
    return 1;
  } else if (redState == 1) {
    // Serial.println("Red Pressed");
    return 2;
  } else {
    // Serial.println("Waiting for Input");
    return 0;
  }
}

int random_color() {
  int idx = rand() % list_size; // Generate a random index
  int random_color = color_list[idx];
  return random_color;
}

void play_existing_sounds(int played_array_size) {
  // Need to light the LED too
  for (int n=0; n<=played_array_size; n++) {
    toggle_led(played_array[n], 1);
    play_sound(played_array[n]);
    toggle_led(played_array[n], 0);
  }
}

void play_sound(int color_number) {
  switch (color_number) {
      case 1:
          note(0, 4);
          break;
      case 2:
          note(4, 4);
          break;
      case 3:
          note(8, 4);
          break;
      case 4:
          note(12, 4);
          break;
      // default:
      //     // Should never get here
  }
}

void toggle_led(int color_number, int off_on) {
  switch (color_number) {
      case 1:
          digitalWrite(GREEN, off_on);
          break;
      case 2:
          digitalWrite(RED, off_on);
          break;
      case 3:
          digitalWrite(BLUE, off_on);
          break;
      case 4:
          digitalWrite(YELLOW, off_on);
          break;
      // default:
      //     // Should never get here
  }
}

/*
Green = 1 = n(0, 4)
Red = 2 = n(4, 4)
Blue = 3 = n(8, 4)
Yellow = 4 = n(12, 4)
*/

/*
Play a sound at random
Listen for input from button press

If it matches, they are correct, now play 2 consecutive sounds (first is same)
Listen for input from button presses

...

If no match, user got it wrong. Play an incorrect noise, wait 5 seconds, start over.
*/

void loop() {
  greenState = 0;
  redState = 0;
  pressed = 0;
  t = 0;
  while (t < T == 1) {
    pressed = readButtons();
    if (pressed > 0 == 1) {
      play_sound(pressed);
      greenState = 0;
      redState = 0;
      t = 0;
      pressed = 0;
      delay(1000);
      break;
    }
    t++;
  }
}

// void loop() {
//   for (int n=0; n<=MAX_ROUNDS; n++) {
//     to_play = random_color();
//     int played_array_size = sizeof(played_array) / sizeof(played_array[0]);
//     if played_array_size > 0 {
//       play_existing_sounds(played_array_size);
//     };
//     toggle_led(to_play, 1);
//     play_sound(to_play);
//     toggle_led(to_play, 0);
//     played_array[n] = to_play; // Add to array
//     // Wait for input
//     delay(3000);
//   }
//   note(0, 0);
//   delay(10000); // Restarting Game 
// }

const int Output = 1; // Can be 1 or 4

// Cater for 16MHz, 8MHz, or 1MHz clock:
const int Clock = ((F_CPU/1000000UL) == 16) ? 4 : ((F_CPU/1000000UL) == 8) ? 3 : 0;
const uint8_t scale[] PROGMEM = {239,226,213,201,190,179,169,160,151,142,134,127};

void note (int n, int octave) {
  int prescaler = 8 + Clock - (octave + n/12);
  if (prescaler<1 || prescaler>15 || octave==0) prescaler = 0;
  DDRB = (DDRB & ~(1<<Output)) | (prescaler != 0)<<Output;
  OCR1C = pgm_read_byte(&scale[n % 12]) - 1;
  GTCCR = (Output == 4)<<COM1B0;
  TCCR1 = 1<<CTC1 | (Output == 1)<<COM1A0 | prescaler<<CS10;
}