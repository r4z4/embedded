// #include <Wire.h>
// #include  <Adafruit_BMP280.h>

// Adafruit_BMP280  bmp;

// void setup() {
//   Serial.begin(115200);
//   delay(1000);
//   bmp.begin(0x76); // I2C Address. BMP has two. Will get NAN if incorrect.
//   delay(2000);
// }

#include <SoftwareSerial.h>
#include <TinyWireM.h>
int ledPin = 4;
int slave_addr = 0x76;
SoftwareSerial mySerial(1, 2); //(Rx, Tx)
void setup() {
  pinMode(ledPin, OUTPUT);
  TinyWireM.begin();        // join i2c bus (address optional for master)
  mySerial.begin(9600);  // start serial for output
  mySerial.print("Requesting data");
}

void loop() {
  digitalWrite(ledPin, LOW);
  TinyWireM.requestFrom(slave_addr, 1);    // request 1 bytes from slave
  //slave_addr can be an array here of many addresses to request

  while (TinyWireM.available()) { // slave may send less than requested
    char c = TinyWireM.read(); // receive a byte as character
    mySerial.print(c);         // print the character
    digitalWrite(ledPin, HIGH);
  }

  delay(500);
  mySerial.println("loop");
}