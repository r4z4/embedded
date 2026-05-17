#include <LiquidCrystal.h>

const int RS = 33, EN = 13, D4 = 14, D5 = 27, D6 = 26, D7 = 25;
LiquidCrystal lcd(RS, EN, D4, D5, D6, D7);

void setup() {
  lcd.begin(16, 2);
  // Print message to LCD
  lcd.print("First time");
  lcd.setCursor(0, 1);
  lcd.print("Second time");
}

void loop() {
}
