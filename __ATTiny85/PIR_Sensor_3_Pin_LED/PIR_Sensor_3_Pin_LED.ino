  // ATTiny85 Pin assignments
  // Pin 2 (PB2) is INT0 - the only pin with hardware external interrupt
  const int led = 0;           // PB0 for LED
  const int motionSensor = 2;  // PB2 (INT0) for PIR sensor

  // Timer: Auxiliary variables
  unsigned long now = millis();
  unsigned long lastTrigger = 0;
  boolean startTimer = false;
  boolean motion = false;

  // Checks if motion was detected, sets LED HIGH and starts a timer
  void detectsMovement() {
    digitalWrite(led, HIGH);
    delay(500);
    digitalWrite(led, LOW);
  }

  void setup() {
    // PIR Motion Sensor mode INPUT_PULLUP
    pinMode(motionSensor, INPUT_PULLUP);

    // Set motionSensor pin as interrupt, assign interrupt function and set RISING mode
    attachInterrupt(0, detectsMovement, RISING);  // INT0 is interrupt 0 on ATTiny85

    // Set LED to LOW
    pinMode(led, OUTPUT);
    digitalWrite(led, LOW);
  }

  void loop() {
  }
