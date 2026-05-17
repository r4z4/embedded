// Use only one core when creating tasks
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// Task: Perform mundane task
void testTask(void *parameter) {
  while(1) {
    int a = 1;
    int b[100]; // int - 4 bytes. So this is 400 bytes

    // Do something with array so it is not optimized out by compiler
    for (int i = 0; i < 100; i++) {
      b[i] = a + 1;
    }
    Serial.println(b[0])

    Serial.print("High Water Mark (words): ");
    Serial.println(uxGetStackHighWaterMark(NULL));

    Serial.print("Heap before malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());

    int *ptr = (int*)pvPortMalloc(1024 - sizeof(int)); // Regular malloc is not thread safe. use pvPortMalloc
    
    if (ptr == NULL) {
        Serial.print("Not enough Heap")
    } else {
        // Do something with array so it is not optimized out by compiler
        for (int i = 0; i < 1024; i++) {
          ptr[i] = 3;
        }
    }

    Serial.print("Heap after malloc (bytes): ");
    Serial.println(xPortGetFreeHeapSize());
    // Free up memory
    vPortFree(ptr);
    // Wait for a bit to observe
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Configure low serial baud rate so we can watch the preemption
  Serial.begin(115200);

  // Wait a moment to start (so we do not miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Memory Allocation---");

  // Start the only other task
  xTaskCreatePinnedToCore(testTask,       // Function to be called
                          "Test Task",    // Name of task
                          1500,           // Stack size (bytes in ESP32, words in FreeRTOS)
                          NULL,           // Parameter to pass to function
                          1,              // Task Priority (o to configMAX_PRIORITY)
                          NULL,        // Task handle
                          app_cpu);       // Define CPU core we want to task to run in.
  
  // Delete setup and loop task
  vTaskDelete(NULL);


void loop() {}
