
#include <driver/i2s.h>

// you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
// most microphones will probably default to left channel but you may need to tie the L/R pin low
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// either wire your microphone to the same pins or change these to match your wiring
#define I2S_MIC_SERIAL_CLOCK D2
#define I2S_MIC_LEFT_RIGHT_CLOCK D0
#define I2S_MIC_SERIAL_DATA D1

// RGB LED pins - adjust these to match your wiring
#define LED_RED D3
#define LED_GREEN D4
#define LED_BLUE D5

void displayColor(int redValue, int greenValue, int blueValue) {
  // Apply PWM (Pulse-Width Modulation) to each pin to control the color intensity
  analogWrite(LED_RED, redValue);
  analogWrite(LED_GREEN, greenValue);
  analogWrite(LED_BLUE, blueValue);
}

// Whistle: Freq: 1695.3 Hz, Amp: 103638496
// Chatter: Freq: 304.7 Hz, Amp: 105763552
// Scream:  742.2 Hz, Amp: 106711040

// Detection thresholds - you may need to tune these based on your microphone
#define WHISTLE_FREQ_MIN 1500    // Whistles are typically 2-4 kHz
#define WHISTLE_FREQ_MAX 2000
#define CHATTER_FREQ_MIN 300    
#define CHATTER_FREQ_MAX 500    
#define SCREAM_FREQ_MAX  1000
#define SCREAM_AMPLITUDE 500000000  // High amplitude threshold for screams
#define CHATTER_AMPLITUDE 100000000 // Lower threshold for normal speech

// don't mess around with this
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};

void setLED(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);
  digitalWrite(LED_BLUE, blue ? HIGH : LOW);
}

// Calculate zero-crossing rate to estimate frequency
float calculateZeroCrossingRate(int32_t* samples, int count) {
  int zeroCrossings = 0;
  for (int i = 1; i < count; i++) {
    if ((samples[i-1] >= 0 && samples[i] < 0) || 
        (samples[i-1] < 0 && samples[i] >= 0)) {
      zeroCrossings++;
    }
  }
  // Frequency = (zero crossings / 2) * (sample_rate / buffer_size)
  return (zeroCrossings / 2.0) * ((float)SAMPLE_RATE / count);
}

// Calculate RMS amplitude
float calculateRMS(int32_t* samples, int count) {
  long long sum = 0;
  for (int i = 0; i < count; i++) {
    long long val = samples[i];
    sum += val * val;
  }
  return sqrt(sum / count);
}

void setup()
{
  // we need serial output for the plotter
  Serial.begin(115200);
  // start up the I2S peripheral
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
  
  // Setup RGB LED pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);
  
  // Start with LED off
  setLED(false, false, false);
}

int32_t raw_samples[SAMPLE_BUFFER_SIZE];
void loop()
{
  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);
  
  // Calculate audio features
  float frequency = calculateZeroCrossingRate(raw_samples, samples_read);
  float amplitude = calculateRMS(raw_samples, samples_read);
  
  // Detect sound type and set LED accordingly
  if (frequency >= WHISTLE_FREQ_MIN && frequency <= WHISTLE_FREQ_MAX) {
    displayColor(255, 255, 0); // Yellow
    Serial.printf("WHISTLE detected! Freq: %.1f Hz, Amp: %.0f\n", frequency, amplitude);
  } 
  else if (frequency > CHATTER_FREQ_MAX && frequency <= SCREAM_FREQ_MAX) {
    displayColor(255, 0, 0);   // Red
    Serial.printf("SCREAM detected! Freq: %.1f Hz, Amp: %.0f\n", frequency, amplitude);
  } 
  else if (frequency >= CHATTER_FREQ_MIN && frequency <= CHATTER_FREQ_MAX) {
    displayColor(0, 255, 0);   // Green
    Serial.printf("CHATTER detected! Freq: %.1f Hz, Amp: %.0f\n", frequency, amplitude);
  } 
  else {
    // Silence or very quiet - LED OFF
    displayColor(0, 0, 0);
  }
  
  // Optional: Still dump samples for serial plotter (comment out if not needed)
  // for (int i = 0; i < samples_read; i++)
  // {
  //   Serial.printf("%ld\n", raw_samples[i]);
  // }
}
