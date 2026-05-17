#include "Env.h"
#include <Wire.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include "esp_now.h"

#define MAX 120
#define MAX_MQTT_CONN_ATTEMPTS 5

TaskHandle_t Task1;
TaskHandle_t Task2;

typedef struct data {
  char* topic;
  char* entry;
} Data;

Data data;

char* gyroTopic = "ESP32_Gyro";
char* rgbTopic = "ESP32_RGB";

// Receiver MAC = Station MAC: 00:4B:12:2E:86:94
uint8_t receiverMAC[] = {0x00, 0x4B, 0x12, 0x2E, 0x86, 0x94}; // Hex

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASS;

const char* mqtt_server = MQTT_Pixel_Server;
const char* mqtt_username = MQTT_USER; // Auth Token
const char* mqtt_password = MQTT_PASS;

const char* mqtt_client_name = "ESP32_Gyro_RGB";

// Use this to track retries
int num_mqtt_connection_attempts = 0;

WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void try_reconnect() {
  Serial.print("Attempting MQTT connection (Attempt #");
  Serial.print(num_mqtt_connection_attempts);
  Serial.print(")...");
  // Attempt to connect
  if (client.connect(mqtt_client_name, mqtt_username, mqtt_password)) {
    Serial.println("connected");
  } else {
    Serial.print("failed, rc=");
    Serial.println(client.state());
  }
}

// Set up the rgb led names (30 Pin)
uint8_t ledR = 15;
uint8_t ledG = 2;
uint8_t ledB = 4;

const int MPU=0x68; 
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

void  setup(){
  Serial.begin(115200);
  delay(10);
  setup_wifi();
  client.setServer(mqtt_server, 1883);

  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);

  if (esp_now_init() != ESP_OK) {
    Serial.println("ESP NOW Init Failed");
    return;
  }
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
  //Create task that'll be executed in Task1code() function, w/ priority 1 & executed on core 0
  xTaskCreatePinnedToCore(
                    gyroTask,    /* Task function. */
                    "gyroTask",  /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //Create task that'll be executed in Task2code() function, w/ priority 1 & executed on core 1
  xTaskCreatePinnedToCore(
                    rgbTask,     /* Task function. */
                    "rgbTask",   /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

void gyroTask( void * pvParameters ){
  for(;;) {
    Wire.beginTransmission(MPU);
    Wire.write(0x3B);  
    Wire.endTransmission(false);
    Wire.requestFrom(MPU,12,true);  
    AcX=Wire.read()<<8|Wire.read();    
    AcY=Wire.read()<<8|Wire.read();  
    AcZ=Wire.read()<<8|Wire.read();  
    GyX=Wire.read()<<8|Wire.read();  
    GyY=Wire.read()<<8|Wire.read();  
    GyZ=Wire.read()<<8|Wire.read();  
    
    Serial.print("Accelerometer: ");
    Serial.print("X = "); Serial.print(AcX);
    Serial.print(" | Y = "); Serial.print(AcY);
    Serial.print(" | Z = ");  Serial.println(AcZ); 
    
    Serial.print("Gyroscope: ");
    Serial.print("X  = "); Serial.print(GyX);
    Serial.print(" | Y = "); Serial.print(GyY);
    Serial.print(" | Z = "); Serial.println(GyZ);
    Serial.println(" ");

    char full_message[MAX];
    // %ld for signed long
    snprintf(full_message, sizeof(full_message), "AcX:%ld;AcY:%ld;AcZ:%ld||GyX:%ld;GyY:%ld;GyZ:%ld", AcX, AcY, AcZ, GyX, GyY, GyZ);
    // client.publish("esp32/gyro", full_message);

    data.topic = gyroTopic;
    data.entry = full_message;

    esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

    vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for 1 second
  }
  vTaskDelete(NULL); // Tasks must be gracefull shutdown
}

void rgbTask( void * pvParameters ){
  for(;;) {
    int redValue = random(0, 256);   // random() generates a number from min (inclusive) to max (exclusive)
    int greenValue = random(0, 256); 
    int blueValue = random(0, 256);

    analogWrite(ledR, redValue);
    analogWrite(ledG, greenValue);
    analogWrite(ledB, blueValue);

    char full_message[MAX];
    // %ld for signed long
    snprintf(full_message, sizeof(full_message), "%u, %u, %u", redValue, greenValue, blueValue);
    // client.publish("esp32/gyro", full_message);

    data.topic = rgbTopic;
    data.entry = full_message;

    esp_now_send(receiverMAC, (uint8_t *)&data, sizeof(data));

    vTaskDelay(500 / portTICK_PERIOD_MS); // Delay for 1 second
  }
  vTaskDelete(NULL);
}

void loop(){}