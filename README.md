# TEIDE EQ: Compact Audio Spectrum Analyzer (ESP32 + OLED)

This project implements a real-time, 8-band audio spectrum analyzer (Equalizer) by performing a Fast Fourier Transform (FFT) on audio data captured by a digital I2S microphone, visualizing the output on an OLED screen.

---

## 🛠️ Hardware & Verified Pinout

This list confirms the successful wiring used in the final working sketch.

| Component | Pin Function | ESP32 GPIO | Notes |
| :--- | :--- | :--- | :--- |
| **Display (OLED)** | SDA (Data) | **GPIO 21** | I2C Address: 0x3C |
| **Display (OLED)** | SCL (Clock) | **GPIO 22** | |
| **Mic (INMP441)** | SCK (BCLK) | **GPIO 14** | Bit Clock |
| **Mic (INMP441)** | WS (LRCL) | **GPIO 15** | Word Select (Required for I2S) |
| **Mic (INMP441)** | SD (Data In) | **GPIO 32** | Serial Data |
| **Mic (INMP441)** | L/R | **GND Rail** | Configured as Left Channel |

---

## 📚 Required Libraries

The following must be installed via the Arduino IDE Library Manager:

1.  **U8g2** (by olikraus) - For the SSD1309 128x64 OLED display control.
2.  **arduinoFFT** - For all Fast Fourier Transform calculations.
3.  **ESP32 Core** - Handles the I2S microphone driver natively.

---

## 🚧 Next To-Dos

A short list of tasks for the next development sprint:

1.  **Integrate Rotary Encoder (KY-040):** Implement the `updateEncoder()` function to map the knob to the audio **sensitivity/gain** variable.
    * *Reserved Encoder Pins:* CLK (GPIO 18), DT (GPIO 19), SW (GPIO 5).
2.  **Refine Graphics:** Implement the Peak Hold logic (using the `barPeak[]` array) to display a small, slowly decaying dot at the maximum bar height.
3.  **Start RGB Matrix Project:** Prepare the hardware and separate code base for the high-power **ESP32-S3 + RGB Matrix** version.