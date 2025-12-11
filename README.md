TEIDE: ESP32 Audio Spectrum Visualizer (SSD1309 + I2S Mic + Rotary Encoder)

A compact real-time audio spectrum visualizer built on ESP32-S3, displaying FFT bars on a 2.42” SSD1309 OLED.
It uses an I2S microphone for audio capture and a rotary encoder to adjust sensitivity.
Perfect for vinyl stands, music displays, desk gadgets, and ambient visualizers.

⸻

✨ Features
	•	Real-time audio spectrum visualization (FFT-based)
	•	24-bar spectrum display with smoothing & peak dynamics
	•	Sensitivity control using a rotary encoder (KY-040)
	•	Startup splash screen
	•	OLED display driven via I2C (U8g2)
	•	I2S microphone input (INMP441 / MAX9814 with I2S mode)
	•	Highly customizable animation logic

⸻

📦 Hardware Used

Component	Model
Microcontroller	ESP32-S3 DevKitC-1 (N16R8 recommended)
Display	2.42” OLED SSD1309 (128×64, I2C)
Microphone	INMP441 (or MAX9814 in I2S configuration)
Rotary Encoder	KY-040
Jumper Wires	Dupont
Power	USB-C


⸻

🔌 Wiring

OLED Display (SSD1309, I²C)

OLED Pin	ESP32-S3 Pin
VCC	5V
GND	GND
SCL	GPIO 14
SDA	GPIO 15
RES	Not connected (U8g2 handles soft reset)


⸻

I2S Microphone

#define I2S_WS  15   // LRCLK
#define I2S_SCK 14   // BCLK
#define I2S_SD  32   // Data

Microphone Pin	ESP32-S3 Pin
WS / L/R CLK	GPIO 15
SCK / BCLK	GPIO 14
SD	GPIO 32
VCC	3.3V
GND	GND


⸻

Rotary Encoder (KY-040)

#define PIN_CLK 35
#define PIN_DT  34
#define PIN_SW  33

Encoder Pin	ESP32-S3 Pin
CLK	35
DT	34
SW	33
+	3.3V
GND	GND


⸻

🛠 Installation Guide

1. Install Arduino IDE

Download from:
https://www.arduino.cc/en/software

2. Install ESP32 Board Package

Arduino IDE → Preferences → Additional Boards URLs:

https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

Tools → Board → Boards Manager
Search for ESP32, install Espressif Systems package.

⸻

📚 Required Libraries

Install via Arduino Library Manager:
	•	U8g2
	•	ArduinoFFT
	•	ESP32 I2S (built in)

⸻

⚙️ Board Configuration

Use:

Board: ESP32S3 Dev Module
USB CDC on Boot: Enabled
USB Mode: CDC Only
CPU Speed: 240 MHz
Upload Speed: 921600

Select your USB port under:
Tools → Port

⸻

⬆️ Uploading the Firmware
	1.	Connect ESP32-S3 via USB-C
	2.	Open the .ino project
	3.	Click Upload
	4.	If upload fails, hold BOOT button on the ESP32 during flashing

⸻

📁 Repository Structure

/src
   main.ino
   visualizer.h
   fft_processing.h
   encoder.h
/assets
   wiring.png (optional)
README.md


⸻

🎚 Sensitivity Control

The rotary encoder adjusts FFT gain:
	•	Clockwise → increases sensitivity
	•	Counter-clockwise → decreases sensitivity

Current sensitivity value is displayed on the OLED.

⸻

🖥 Visualization
	•	24 bars (frequency-scaled)
	•	Smoothing filter prevents jitter
	•	Optional logarithmic FFT bin grouping
	•	Peak hold and decay system
	•	Customizable visualization styles (future)

⸻

🚀 Future Improvements
	•	Multiple visualization modes
	•	Button press to switch modes
	•	Color display option (SPI TFT)
	•	Bluetooth audio input
	•	MicroSD logging

⸻

📸 Demo

(Add your real images here)

![demo](assets/demo1.jpg)
![demo](assets/demo2.jpg)


⸻

👨‍💻 Development Tips
	•	To modify responsiveness, adjust:
	•	FFT scaling
	•	smoothing factor
	•	bar decay
	•	frequency grouping
	•	Code is modular—easy to swap visualization methods
	•	Best testing is with strong bass content (sine sweeps help)

⸻

📜 License

MIT License (or your choice)

⸻

If you’d like, I can also generate:

✅ wiring diagram PNG
✅ architecture diagram (FFT → smoothing → display)
✅ short “contributing guide”
✅ .gitignore and folder template

Just say “generate the diagram” or “make contributing.md” and I’ll do it.