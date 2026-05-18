/*
  =========================================
  PROJECT  : Pendeteksi Kebisingan ESP32
  SENSOR   : INMP441
  OUTPUT   : LED + BUZZER
  BOARD    : ESP32
  =========================================
*/

#include <driver/i2s.h>

// ================= PIN =================
#define I2S_WS    15
#define I2S_SD    32
#define I2S_SCK   14

#define LED_PIN      2
#define BUZZER_PIN   4

// ================= I2S =================
#define I2S_PORT I2S_NUM_0
#define BUFFER_LEN 128

int16_t samples[BUFFER_LEN];

// Threshold kebisingan
int threshold = 2000;

// =======================================
// SETUP I2S
// =======================================
void setupI2S() {

  const i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 128,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  // Install driver
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  // Set pin
  i2s_set_pin(I2S_PORT, &pin_config);

  // Clear DMA
  i2s_zero_dma_buffer(I2S_PORT);
}

// =======================================
// SETUP
// =======================================
void setup() {

  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  setupI2S();

  Serial.println("=================================");
  Serial.println(" Monitoring Kebisingan Aktif ");
  Serial.println("=================================");
}

// =======================================
// LOOP
// =======================================
void loop() {

  size_t bytesRead = 0;

  // Baca data microphone
  i2s_read(
    I2S_PORT,
    &samples,
    sizeof(samples),
    &bytesRead,
    portMAX_DELAY
  );

  int samplesRead = bytesRead / sizeof(int16_t);

  long total = 0;

  // Hitung rata-rata amplitudo
  for (int i = 0; i < samplesRead; i++) {
    total += abs(samples[i]);
  }

  int noiseLevel = total / samplesRead;

  // Tampilkan ke Serial Monitor
  Serial.print("Noise Level : ");
  Serial.println(noiseLevel);

  // =====================================
  // DETEKSI KEBISINGAN
  // =====================================
  if (noiseLevel > threshold) {

    digitalWrite(LED_PIN, HIGH);

    // bunyi buzzer
    tone(BUZZER_PIN, 2000);

    Serial.println("STATUS : BISING");

  } else {

    digitalWrite(LED_PIN, LOW);

    noTone(BUZZER_PIN);

    Serial.println("STATUS : AMAN");
  }

  delay(100);
}