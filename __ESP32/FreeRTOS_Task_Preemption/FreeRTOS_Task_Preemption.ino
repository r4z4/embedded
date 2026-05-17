// Use only one core when creating tasks
#if CONFIG_FREERTOS_UNICORE
static const BaseType_t app_cpu = 0;
#else
static const BaseType_t app_cpu = 1;
#endif

// String to print
const char msg[] = "Preemption in action!";

// Task Handles
static TaskHandle_t task_1 = NULL;
static TaskHandle_t task_2 = NULL;

//*******************************************
// Tasks

// Task: Print to Serial Terminal w/ lower priority
void startTask1(void *parameter) {
  // Count # of chars in string
  int msg_len = strlen(msg);

  // Print string to Terminal
  while (1) {
    Serial.println();
    for (int i = 0; i < msg_len; i++) {
      Serial.print(msg[i]);
    }
    Serial.println();
    vTaskDelay(1000 / portTICK_PERIOD_MS); // Put task into blocked state for 1 sec
  }
}

// Task: Print to Serial Terminal w/ higher priority
void startTask2(void *parameter) {
  while(1) {
    Serial.print('*');
    vTaskDelay(1 / portTICK_PERIOD_MS);  // Even a minimal delay helps. Without this it never yields back.
  }
}

//****************************************************
// Main (runs as its own task with priority 1 on core 1)

void setup() {
  // Configure low serial baud rate so we can watch the preemption
  Serial.begin(300);

  // Wait a moment to start (so we do not miss Serial output)
  vTaskDelay(1000 / portTICK_PERIOD_MS);
  Serial.println();
  Serial.println("---FreeRTOS Task Preemption---");

  // Print self priority
  Serial.print("Setup and loop task running on core ");
  Serial.print(xPortGetCoreID());
  Serial.print(" with priority ");
  Serial.println(uxTaskPriorityGet(NULL));

  // Task to run forever
  xTaskCreatePinnedToCore(startTask1,     // Function to be called
                          "Task 1",       // Name of task
                          1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
                          NULL,           // Parameter to pass to function
                          1,              // Task Priority (o to configMAX_PRIORITY)
                          &task_1,        // Task handle
                          app_cpu);       // Define CPU core we want to task to run in.
  
  // Task to run once with higher priority
  xTaskCreatePinnedToCore(startTask2,     // Function to be called
                          "Task 2",       // Name of task
                          1024,           // Stack size (bytes in ESP32, words in FreeRTOS)
                          NULL,           // Parameter to pass to function
                          2,              // Task Priority (o to configMAX_PRIORITY)
                          &task_2,        // Task handle
                          app_cpu);       // Define CPU core we want to task to run in.

}

void loop() {
  // Suspend the higher priorty task for some intervals
  for (int i = 0; i < 3; i++) {
    vTaskSuspend(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    vTaskResume(task_2);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
  }

  // Delete the lower priority task
  if (task_1 != NULL) {
    vTaskDelete(task_1); // If call delete on non-existant task, its bad. Overwrite memory or crash.
    task_1 = NULL;
  }
}
