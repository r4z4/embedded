// #include <DigisparkOLED.h>
#include <TinyWireM.h>
#include <Tiny4kOLED.h>
//---------------------------------------------------
unsigned int i =
 0;
//===================================================
void setup()
{
  oled.begin();
  oled.clear();
  oled.setFont(FONT8X16);
  oled.on(); // Don't forget this
}
//===================================================
void loop()
{   
  i++;
  //-------------------------------------------------
  oled.setFont(FONT8X16);
  oled.setCursor(10, 0);
  oled.print("Counter: ");
  oled.println(i);
  delay(2000);
}