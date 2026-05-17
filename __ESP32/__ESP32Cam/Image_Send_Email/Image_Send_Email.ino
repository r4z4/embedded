#include "WiFi.h"
#include "esp_camera.h"
#include <ESP_Mail_Client.h>
#include <driver/rtc_io.h>
#include "Env.h"

// ==========================================
// Replace with your Network Credentials
// ==========================================
#define WIFI_SSID WIFI_SSID
#define WIFI_PASSWORD WIFI_PASS

// ==========================================
// Replace with your Email Credentials
// ==========================================
#define SMTP_HOST "smtp.mail.yahoo.com"
#define SMTP_PORT 465
#define AUTHOR_EMAIL EMAIL_SENDER_ADDR
#define AUTHOR_PASSWORD EMAIL_SENDER_PASS
#define RECIPIENT_EMAIL EMAIL_RECIPIENT_ADDR
// Your Mint Mobile Number (10 digits) + @tmomail.net
#define RECIPIENT_PHONE_EMAIL PHONE_EMAIL_RECIPIENT_ADDR

// SMTP Session object used for Email sending
SMTPSession smtp;

// ==========================================
// Camera pins for AI Thinker ESP32-CAM
// ==========================================
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ==========================================
// Deep Sleep Configuration
// ==========================================
#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  180         // 3 minutes - for testing
// #define TIME_TO_SLEEP  21600       // Time ESP32 will go to sleep (in seconds). 6 hours = 21600 seconds

// Function declaration
void smtpCallback(SMTP_Status status);
void goToSleep();
void sendEmail(camera_fb_t * fb);

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // 1. Configure camera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Allocate larger buffers if PSRAM is available
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // 1600x1200
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // Initialize Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    goToSleep();
  }
  
  // 2. Take Picture
  camera_fb_t * fb = NULL;
  
  // Often it's recommended to discard the first few frames as the sensor adjusts its exposition
  for (int i = 0; i < 3; i++) {
    fb = esp_camera_fb_get();
    esp_camera_fb_return(fb);
    delay(200);
  }
  
  fb = esp_camera_fb_get(); // Actual frame we will keep
  if(!fb) {
    Serial.println("Camera capture failed");
    goToSleep();
  }

  // 3. Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to Wi-Fi!");
    
    // 4. Time to send out the captured frame via Email/MMS
    sendEmail(fb);
    
  } else {
    Serial.println("\nWiFi connection failed.");
  }

  // Return the frame buffer back to be reused
  esp_camera_fb_return(fb);
  
  // 5. Go to Deep Sleep
  goToSleep();
}

void loop() {
  // Empty because execution stops when esp_deep_sleep_start() is called in setup()
}

void goToSleep() {
  Serial.println("Going to sleep now...");
  
  // Configure the timer to wake us up in 6 hours
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  
  // Turn off the Flash LED to save power, and hold its state outputting low during deep sleep
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  rtc_gpio_hold_en(GPIO_NUM_4);
  
  delay(100);
  // Start deep sleep
  esp_deep_sleep_start();
}

void sendEmail(camera_fb_t * fb) {
  // Set the SMTP Callback
  smtp.callback(smtpCallback);

  // Setup session variables
  ESP_Mail_Session session;
  session.server.host_name = SMTP_HOST;
  session.server.port = SMTP_PORT;
  session.login.email = AUTHOR_EMAIL;
  session.login.password = AUTHOR_PASSWORD;
  session.login.user_domain = "";

  // Set message details
  SMTP_Message message;
  message.sender.name = "ESP32-CAM";
  message.sender.email = AUTHOR_EMAIL;
  message.subject = "New Photo captured from ESP32-CAM";
  message.addRecipient("Owner", RECIPIENT_EMAIL);
  
  message.text.content = "Attached is the latest photo from your ESP32-CAM taken inside the sleep cycle.";
  
  // Attach the picture directly from the frame buffer
  SMTP_Attachment att;
  att.descr.filename = "photo.jpg";
  att.descr.mime = "image/jpeg";
  att.blob.data = fb->buf;
  att.blob.size = fb->len;
  message.addInlineImage(att);

  // Connect to the server
  if (!smtp.connect(&session)) {
    Serial.println("Failed to connect to SMTP server");
    return;
  }
    
  // Send Email
  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.println("Error sending Email: " + smtp.errorReason());
  }
}

// Callback function to get the Email sending status
void smtpCallback(SMTP_Status status) {
  Serial.println(status.info());
  if (status.success()) {
    Serial.println("Email successfully sent.");
    smtp.sendingResult.clear();
  }
}
