#include <Arduino.h>
#include <IRremote.hpp> // Make sure to install the "IRremote" library in the Library Manager

// Define the signal pin for the IR receiver. 
// Change this to match the GPIO pin you connected the data line (OUT) to on your ESP32-C3 Super Mini.
#define IR_RECEIVE_PIN 2

// RGB Buttons on LED Remote
// int blue = 0xA455AA50
// int green = 0x85279A8C
// int red = 0xB49350EC

int receivedHex;

// Buttons on Generic IR Remote from ElectroPeak
int btn1 = 0xBA45FF00;
int btn2 = 0xB946FF00;
int btn3 = 0xB847FF00;
int btn4 = 0xBB44FF00;
int btn5 =  0xBF40FF00;
int btn6 = 0xBC43FF00;
int btn7 = 0xF807FF00;
int btn8 = 0xEA15FF00;
int btn9 = 0xF609FF00;
int btnStar =  0xE916FF00;
int btn0 = 0xE619FF00;
int btnPound = 0xF20DFF00;
int btnUp = 0xE718FF00;
int btnRight = 0xA55AFF00;
int btnDown = 0xAD52FF00;
int btnLeft = 0xF708FF00;
int btnOK = 0xE31CFF00;

const int LED1Pin = 21;
const int LED2Pin = 20;

void setup() {
    // Initialize serial communication at 115200 baud rate typical for ESP32
    Serial.begin(115200);
    
    // Wait for the serial port to be ready (useful for native USB on ESP32-C3)
    while (!Serial) {
        delay(10);
    }

    pinMode(LED1Pin, OUTPUT);
    pinMode(LED2Pin, OUTPUT);
    
    Serial.println("IR Receiver starting...");
    
    // Start the receiver. DISABLE_LED_FEEDBACK prevents a known crash on ESP32-C3 boards that don't map LED_BUILTIN correctly.
    IrReceiver.begin(IR_RECEIVE_PIN, DISABLE_LED_FEEDBACK);
    
    Serial.print("Ready to receive IR signals. Listening on GPIO: ");
    Serial.println(IR_RECEIVE_PIN);
}

void loop() {
    // Check if we have received an IR signal
    if (IrReceiver.decode()) {
        
        // Print the raw data as HEX
        Serial.print("Data (HEX): 0x");
        receivedHex = IrReceiver.decodedIRData.decodedRawData;
        Serial.println(receivedHex, HEX);
        
        // Print a short, human-readable summary of the received signal (Protocol, Address, Command)
        IrReceiver.printIRResultShort(&Serial);

        if (receivedHex == btn1) {
            digitalWrite(LED1Pin, HIGH);
            digitalWrite(LED2Pin, LOW);
        } else if (receivedHex == btn2) {
            digitalWrite(LED1Pin, LOW);
            digitalWrite(LED2Pin, HIGH);
        } else {
            digitalWrite(LED1Pin, LOW);
            digitalWrite(LED2Pin, LOW);
        }
        
        // Wait a short time to debounce/avoid receiving the same signal repeatedly too fast
        delay(150);
        
        // Resume listening for the next signal
        IrReceiver.resume(); 
    }
}
