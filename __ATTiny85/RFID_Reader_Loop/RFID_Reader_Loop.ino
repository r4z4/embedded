#include <SendOnlySoftwareSerial.h>
#include <TinyWireM.h>
int slave_addr = 0x00;
int Tx = 4;
SendOnlySoftwareSerial mySerial(Tx); //Transmit pin
void setup() {
  TinyWireM.begin();        // join i2c bus (address optional for master)
  mySerial.begin(9600);  // start serial for output
  mySerial.print("Requesting data");
}

void loop() {
  TinyWireM.requestFrom(slave_addr, 1);    // request 1 bytes from slave
  //slave_addr can be an array here of many addresses to request

  while (TinyWireM.available()) { // slave may send less than requested
    char c = TinyWireM.read(); // receive a byte as character
    mySerial.print(c);         // print the character
  }

  delay(500);
  mySerial.println("loop");
}