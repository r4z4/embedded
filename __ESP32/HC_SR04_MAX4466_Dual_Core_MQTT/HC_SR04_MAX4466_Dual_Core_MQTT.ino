#include "Env.h"
#include <WiFi.h>
#include <PubSubClient.h>
#define MAX 100 
const int trigPin = 32;
const int echoPin = 35;
const int soundPin = 34;

const char *ssid = WIFI_SSID; // Enter your WiFi name
const char *password = WIFI_PASS;  // Enter WiFi password

TaskHandle_t Task1;
TaskHandle_t Task2;
#define MAX 120
#define MAX_MQTT_CONN_ATTEMPTS 5

const int sampleWindow = 50;  // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
// MQTT
const char* mqtt_server = MQTT_Pixel_Server;
const char* mqtt_username = "admin"; // Auth Token
const char* mqtt_password = "adminpwd";

const char* mqtt_client_name = "ESP32_HC_SR04";
const char* hcTopic = "esp32/sensor_data_hc_sr04";
const char* maxTopic = "esp32/sensor_data_max4466";

// Use this to track retries
int num_mqtt_connection_attempts = 0;

WiFiClient espClient;
PubSubClient client(espClient);

char hc_message[MAX];
char max_message[MAX];

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
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

void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  // client.setCallback(callback); For receiving messages

  if (!client.connected() && num_mqtt_connection_attempts < MAX_MQTT_CONN_ATTEMPTS) {
    num_mqtt_connection_attempts++;
    try_reconnect();
  };

  //Create task that'll be executed in Task1code() function, w/ priority 1 & executed on core 0
  xTaskCreatePinnedToCore(
                    hcTask,    /* Task function. */
                    "hcTask",  /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task1,      /* Task handle to keep track of created task */
                    0);          /* pin task to core 0 */                  
  delay(500); 

  //Create task that'll be executed in Task2code() function, w/ priority 1 & executed on core 1
  xTaskCreatePinnedToCore(
                    maxTask,     /* Task function. */
                    "maxTask",   /* name of task. */
                    10000,       /* Stack size of task */
                    NULL,        /* parameter of the task */
                    1,           /* priority of the task */
                    &Task2,      /* Task handle to keep track of created task */
                    1);          /* pin task to core 1 */
    delay(500); 
}

void maxTask( void * pvParameters ) {
  for(;;) {
    unsigned long startMillis = millis(); // Start of sample window
    unsigned int peakToPeak = 0;   // peak-to-peak level

    unsigned int signalMax = 0;
    unsigned int signalMin = 1024;

    // collect data for 50 mS and then plot data
    while (millis() - startMillis < sampleWindow)
    {
      sample = analogRead(soundPin);
      if (sample < 1024)  // toss out spurious readings
      {
        if (sample > signalMax)
        {
          signalMax = sample;  // save just the max levels
        }
        else if (sample < signalMin)
        {
          signalMin = sample;  // save just the min levels
        }
      }
    }
    peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
    Serial.print("Sound: ");
    Serial.println(peakToPeak);
    // If we want a voltage representation of the signal
    //double volts = (peakToPeak * 5.0) / 1024;  // convert to volts
    //Serial.println(volts);
    snprintf(max_message, sizeof(max_message), "Sound: %lu", peakToPeak);
    // Publish the string
    client.publish(maxTopic, max_message);

    delay(1000);
  }
}

void hcTask( void * pvParameters ){
  for(;;) {
    long duration, inches, cm;

    // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
    // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    // The same pin is used to read the signal from the PING))): a HIGH pulse
    // whose duration is the time (in microseconds) from the sending of the ping
    // to the reception of its echo off of an object.
    duration = pulseIn(echoPin, HIGH);

    // convert the time into a distance
    inches = microsecondsToInches(duration);
    cm = microsecondsToCentimeters(duration);

    Serial.print(inches);
    Serial.print("in, ");
    Serial.print(cm);
    Serial.print("cm");
    Serial.println();

    snprintf(hc_message, sizeof(hc_message), "In: %lu; Cm: %lu", inches, cm);
    // Publish the string
    client.publish(hcTopic, hc_message);

    delay(1000);
  }
}

void loop() {}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are 73.746
  // microseconds per inch (i.e. sound travels at 1130 feet per second).
  // This gives the distance travelled by the ping, outbound and return,
  // so we divide by 2 to get the distance of the obstacle.
  // See: https://www.parallax.com/package/ping-ultrasonic-distance-sensor-downloads/
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the object we
  // take half of the distance travelled.
  return microseconds / 29 / 2;
}
