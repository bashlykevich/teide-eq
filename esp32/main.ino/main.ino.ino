// =====================================================================
//  ESP32-S3 I2S Audio Visualizer + SSD1309 OLED + KY-040 Encoder
//  Winamp-ish segmented bars + peak hold
// =====================================================================
#include <U8g2lib.h>
#include <Wire.h>
#include "arduinoFFT.h"
#include <driver/i2s.h>

// -------------------- DISPLAY --------------------
// 2.42" SSD1309 128x64 I2C OLED
// SDA -> GPIO 21, SCL -> GPIO 22  (change in Wire.begin if needed)
U8G2_SSD1309_128X64_NONAME2_F_HW_I2C u8g2(U8G2_R0, /* reset =*/ U8X8_PIN_NONE);

// -------------------- ROTARY ENCODER --------------------
#define ENCODER_CLK 35
#define ENCODER_DT  34
#define ENCODER_SW  33

int   lastEncoderCLK = HIGH;
float sensitivity    = 1.0f;      // changed by encoder
const float SENS_MIN = 0.1f;
const float SENS_MAX = 4.0f;

// -------------------- I2S MICROPHONE PINS ----------------
#define I2S_WS   15    // LRCL
#define I2S_SCK  14    // BCLK
#define I2S_SD   32    // DOUT
#define I2S_PORT I2S_NUM_0

// -------------------- AUDIO / FFT SETTINGS ----------------
const uint16_t SAMPLE_RATE = 16000;
const uint16_t SAMPLES     = 512;          // must be power of 2

float vReal[SAMPLES];
float vImag[SAMPLES];

ArduinoFFT<float> FFT(vReal, vImag, SAMPLES, SAMPLE_RATE);

// -------------------- VISUALIZATION --------------------
const uint8_t NUM_BARS  = 16;
const uint8_t SCREEN_W  = 128;
const uint8_t SCREEN_H  = 64;

const uint8_t BAR_WIDTH = 4;
const uint8_t BAR_GAP   = 4;

float barLevel[NUM_BARS];        // per-frame bar height (px)
float barLevelSmooth[NUM_BARS];  // smoothed heights
float barPeak[NUM_BARS];         // peak-hold heights

// =====================================================================
//  I2S SETUP
// =====================================================================
void setupI2S() {
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format =
          (i2s_comm_format_t)(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 4,
      .dma_buf_len   = SAMPLES,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
      .bck_io_num   = I2S_SCK,
      .ws_io_num    = I2S_WS,
      .data_out_num = I2S_PIN_NO_CHANGE,
      .data_in_num  = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

// =====================================================================
//  ENCODER HANDLING
// =====================================================================
void updateEncoder() {
  int currentStateCLK = digitalRead(ENCODER_CLK);

  if (currentStateCLK != lastEncoderCLK) {
    if (digitalRead(ENCODER_DT) != currentStateCLK) {
      // Clockwise
      sensitivity += 0.1f;
    } else {
      // Counter-clockwise
      sensitivity -= 0.1f;
    }
    sensitivity = constrain(sensitivity, SENS_MIN, SENS_MAX);

    Serial.print("Sensitivity: ");
    Serial.println(sensitivity, 2);
  }
  lastEncoderCLK = currentStateCLK;

  // Button press -> reset sensitivity
  static int lastButtonState = HIGH;
  int buttonState = digitalRead(ENCODER_SW);
  if (buttonState == LOW && lastButtonState == HIGH) {
    sensitivity = 1.0f;
    Serial.println("Button pressed -> reset sensitivity to 1.0");
  }
  lastButtonState = buttonState;
}

// =====================================================================
//  AUDIO CAPTURE + FFT
// =====================================================================
void captureAudioAndFFT() {
  int32_t samples[SAMPLES];
  size_t bytes_read = 0;
  i2s_read(I2S_PORT, (void *)samples, sizeof(samples), &bytes_read, portMAX_DELAY);

  uint16_t sampleCount = bytes_read / sizeof(int32_t);
  if (sampleCount > SAMPLES) sampleCount = SAMPLES;

  // Convert samples to floats and apply a Hann window
  for (uint16_t i = 0; i < sampleCount; i++) {
    int32_t s = samples[i] >> 14; // scale down from 32 bits
    float window = 0.5f * (1.0f - cosf((2.0f * PI * i) / (sampleCount - 1)));
    vReal[i] = (float)s * window;
    vImag[i] = 0.0f;
  }
  for (uint16_t i = sampleCount; i < SAMPLES; i++) {
    vReal[i] = 0;
    vImag[i] = 0;
  }

  FFT.windowing(FFTWindow::Rectangle, FFTDirection::Forward); // Hann already applied
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();
}

// =====================================================================
//  BUILD BAR LEVELS FROM FFT  (log-spaced, weighted, smoothed + peaks)
// =====================================================================
void computeBarsFromFFT() {
  uint16_t half = SAMPLES / 2;  // positive frequencies only

  // We visualize roughly 60 Hz – 8 kHz.
  const float minFreq = 60.0f;
  const float maxFreq = 8000.0f;

  // Highest FFT bin we will use (ignore very top end = mic hiss)
  const uint16_t maxBinUsed =
      (uint16_t)(maxFreq * half / (SAMPLE_RATE / 2.0f));

  for (uint8_t b = 0; b < NUM_BARS; b++) {
    // Logarithmic band edges: bass on the left, treble on the right
    float barMinFreq = minFreq * powf(maxFreq / minFreq, (float)b / NUM_BARS);
    float barMaxFreq = minFreq * powf(maxFreq / minFreq, (float)(b + 1) / NUM_BARS);

    uint16_t startBin = (uint16_t)(barMinFreq * half / (SAMPLE_RATE / 2.0f));
    uint16_t endBin   = (uint16_t)(barMaxFreq * half / (SAMPLE_RATE / 2.0f));

    // Clamp into valid and useful range
    if (startBin < 1)           startBin = 1;            // skip DC
    if (endBin   > maxBinUsed)  endBin   = maxBinUsed;   // ignore ultrasonic noise

    // Make sure we have at least one bin
    if (endBin < startBin)      endBin = startBin;

    float sum  = 0.0f;
    uint16_t n = 0;

    for (uint16_t i = startBin; i <= endBin; i++) {
      sum += vReal[i];
      n++;
    }

    float avg = (n > 0) ? sum / n : 0.0f;

    // Per-band weighting.
    // A little extra punch in the lows, neutral mids,
    // and a small lift for highs so the right side doesn’t look dead.
    float t = (float)b / (NUM_BARS - 1);   // 0 = lowest bar, 1 = highest
    float weight;
    if      (t < 0.15f) weight = 1.3f;  // deep bass
    else if (t < 0.30f) weight = 1.15f; // upper bass / low mids
    else if (t < 0.70f) weight = 1.00f; // mids
    else                weight = 1.24f; // highs

    // Convert magnitude → screen height
    float level  = log10f(1.0f + avg) * sensitivity * 20.0f * weight;
    if (level < 0) level = 0;

    float height = level;
    if (height > (SCREEN_H - 10)) height = SCREEN_H - 10;  // keep text area clear

    barLevel[b] = height;
  }

  // Smooth bars (different up/down speeds)
  const float SMOOTH_UP   = 0.8f;   // bars jump up faster
  const float SMOOTH_DOWN = 0.35f;  // bars drop faster

  for (uint8_t b = 0; b < NUM_BARS; b++) {
    if (barLevel[b] > barLevelSmooth[b]) {
      barLevelSmooth[b] =
          barLevelSmooth[b] * (1.0f - SMOOTH_UP) + barLevel[b] * SMOOTH_UP;
    } else {
      barLevelSmooth[b] =
          barLevelSmooth[b] * (1.0f - SMOOTH_DOWN) + barLevel[b] * SMOOTH_DOWN;
    }

    // Peak-hold
    if (barLevelSmooth[b] > barPeak[b]) {
      barPeak[b] = barLevelSmooth[b];
    } else {
      barPeak[b] -= 1.0f;   // faster peak decay
      if (barPeak[b] < 0) barPeak[b] = 0;
    }
  }
}

// =====================================================================
//  DRAW TO OLED  (segmented bars + peak dots)
// =====================================================================
void drawVisualizer() {
  u8g2.clearBuffer();

  // Sensitivity text
  u8g2.setFont(u8g2_font_5x8_tr);
  u8g2.setCursor(0, 8);
  u8g2.print("Sens: ");
  u8g2.print(sensitivity, 1);

  // Draw bars
  const uint8_t maxBarHeight = SCREEN_H - 10;
  const uint8_t segHeight    = 3;
  const uint8_t segGap       = 1;

  uint8_t x = 0;
  for (uint8_t b = 0; b < NUM_BARS; b++) {
    uint8_t h = (uint8_t)barLevelSmooth[b];
    if (h > maxBarHeight) h = maxBarHeight;

    // segmented bar: 3px bar + 1px gap
    for (uint8_t y = 0; y < h; y += (segHeight + segGap)) {
      uint8_t segH = segHeight;
      if (y + segH > h) segH = h - y;
      uint8_t segY = SCREEN_H - y - segH;
      u8g2.drawBox(x, segY, BAR_WIDTH, segH);
    }

    // peak-hold: 1px line on top
    uint8_t peakH = (uint8_t)barPeak[b];
    if (peakH > maxBarHeight) peakH = maxBarHeight;
    uint8_t peakY = SCREEN_H - peakH - 1;
    if (peakH > 0 && peakY < SCREEN_H) {
      u8g2.drawHLine(x, peakY, BAR_WIDTH);
    }

    x += BAR_WIDTH + BAR_GAP;
  }

  u8g2.sendBuffer();
}

// =====================================================================
//  SPLASH SCREEN
// =====================================================================
void showSplash() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tf);
  u8g2.setCursor(0, 16);
  u8g2.print("TEIDE EQ");
  u8g2.setCursor(0, 32);
  u8g2.print("ESP32-S3 + I2S Mic");
  u8g2.setCursor(0, 48);
  u8g2.print("Turn knob = gain");
  u8g2.sendBuffer();
  delay(2000);
}

// =====================================================================
//  SETUP / LOOP
// =====================================================================
void setup() {
  Serial.begin(115200);
  delay(200);

  // Encoder
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT,  INPUT_PULLUP);
  pinMode(ENCODER_SW,  INPUT_PULLUP);
  lastEncoderCLK = digitalRead(ENCODER_CLK);

  // Display
  Wire.begin();          // if different SDA/SCL: Wire.begin(SDA_PIN, SCL_PIN);
  u8g2.begin();
  showSplash();

  // I2S mic
  setupI2S();

  // Init smoothing / peaks
  for (uint8_t i = 0; i < NUM_BARS; i++) {
    barLevelSmooth[i] = 0;
    barPeak[i]        = 0;
  }
}

void loop() {
  updateEncoder();          // read knob & update sensitivity
  captureAudioAndFFT();     // grab samples & run FFT
  computeBarsFromFFT();     // FFT -> bar heights + peaks
  drawVisualizer();         // draw frame
}