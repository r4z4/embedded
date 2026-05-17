// Use only one core when creating tasks
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Pins
static const int ledPin = 23;

// Our Task: Blink an LED
void toggleLED(void *parameter) {
  while(1) {
    digitalWrite(ledPin, HIGH);
    vTaskDelay(500 / portTICK_PERIOD_MS); // Default = 1
    digitalWrite(ledPin, LOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
}

void setup() {
  pinMode(ledPin, OUTPUT);
  // Task to run forever
  xTaskCreatePinnedToCore(      // Use xTaskCreate in vanilla RTOS (not ESP-IDF)
                toggleLED,      // Function to be called
                "Toggle LED",   // Name of task
                1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
                NULL,           // Parameter to pass to function
                1,              // Task Priority (o to configMAX_PRIORITY)
                NULL,           // Task handle
                app_cpu);       // Define CPU core we want to task to run in. Here it is core 1 in our dual-core processor. (ESP32 only)

  // If this was vanilla FreeRTOS, we would want to call xTaskStartScheduler() in
  // main after setting up tasks. But here that function is called for us
  // prior to the setup function, so we do not need to worry about it
}

void loop() {}
