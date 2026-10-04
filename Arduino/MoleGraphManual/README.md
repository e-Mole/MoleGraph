# MoleGraphManual - Arduino Library

Open-source Arduino library for custom firmware development with **MoleGraph** school probeware system.

## 📚 Overview

**MoleGraphManual** is a callback-based library that allows you to:
- ✅ Develop custom sensor firmware with simultaneous data transmission
- ✅ Control external actuators (servos, motors, LEDs, speakers)
- ✅ Create visual programs using **Blockly@rduino** with automatic code generation
- ✅ Build advanced experimental designs requiring custom logic
- ✅ Synchronize measurements with desktop/mobile MoleGraph applications

### When to Use

| Use **MoleGraphManual** | Use **MoleGraphAuto** |
|:---|:---|
| Custom firmware with simultaneous data transmission | Simple plug-and-play measurements |
| Actuator control (servos, motors, etc.) | 30+ built-in sensor types with automatic support |
| Advanced experimental designs | Automatic calibration via desktop app |
| Visual programming (Blockly@rduino) | Minimal custom coding required |

---

## 🔧 Hardware Requirements

### Microcontroller
- **Arduino NANO** (ATmega328P) — primary platform
- Compatible with Arduino UNO or other ATmega328P platforms

### Optional: MoleGraph Shield U01
- 4x RJ12 sensor ports
- Battery voltage monitoring LEDs (red = low, green = ok)
- Status/connection indicator LED
- 4 buttons

### Connectivity
- **USB Serial** (CH340 or FT232 chip) — 115200 baud
- **Bluetooth HC-05 module** (optional) — wireless communication
- **Note:** Arduino NANO has only one hw serial connection; operates in exclusive mode (USB **or** Bluetooth, not simultaneously)

### Supported Sensors
Via I2C, OneWire, PWM, and analog (0–5V) protocols:
- Supported Sensors: 30+ sensor types (temperature, pressure, distance, acceleration, pH, CO₂, oxygen, heart rate, color, UV, etc.)

---

## 💾 Installation

### Option 1: Arduino IDE Library Manager
1. Sketch → Include Library → Manage Libraries
2. Search: `MoleGraphManual`
3. Click Install

### Option 2: Manual Installation
1. Clone or download [e-Mole/MoleGraph](https://github.com/e-Mole/MoleGraph)
2. Copy `Arduino/MoleGraphManual/` to your Arduino libraries folder:
   - **Windows**: `Documents\Arduino\libraries\`
   - **macOS**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
3. Restart Arduino IDE

### Option 3: PlatformIO
```ini
lib_deps =
    e-Mole/MoleGraph
```

---

## 🚀 Quick Start

### Minimal Example (No Shield)

```cpp
#include <molegraphmanual.h>

MoleGraphManual moleGraph;

void updateGraphChannels(void) {
  int value = analogRead(A0);
  moleGraph.setChannelValue(1, value);
}

void setup() {
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
}

void loop() {
  moleGraph.process();  // Handle serial commands & timing
}
```

### With MoleGraph Shield U01

```cpp
#define SYSTEM  // Enable shield hardware features (battery monitoring, LEDs, buttons)
#include <molegraphmanual.h>

MoleGraphManual moleGraph;

void updateGraphChannels(void) {
  // Use shield port definitions (PORT_1A, PORT_2A, etc.)
  int raw = analogRead(PORT_1A);
  moleGraph.setChannelValue(1, raw);
}

void setup() {
  moleGraph.init();  // Initializes serial, timer, and shield hardware
  moleGraph.setSendingCallback(&updateGraphChannels);
}

void loop() {
  moleGraph.process();  // Handles serial commands, updates LEDs, monitors battery
}
```

### With Servo Control

```cpp
#define SYSTEM
#include <molegraphmanual.h>

MoleGraphManual moleGraph;
Servo myServo;

void updateGraphChannels(void) {
  int sensorValue = analogRead(PORT_1A);
  int servoAngle = map(sensorValue, 0, 1023, 0, 180);
  myServo.write(servoAngle);
  moleGraph.setChannelValue(1, sensorValue);
}

void setup() {
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  myServo.attach(2);  // Servo on digital pin 2
}

void loop() {
  moleGraph.process();
}
```

---

## 📌 PIN Assignments (Shield U01)

Each of 4 ports provides: **Analog, Digital, Pull-up**

| Port | Analog | Digital | Pull-up |
|:----:|:------:|:-------:|:-------:|
|  1   |   14   |   11    |    7    |
|  2   |   15   |   10    |    6    |
|  3   |   16   |    9    |    5    |
|  4   |   19   |    8    |    4    |

### Special Pins (with `#define SYSTEM`)

| Feature | Pin |
|:--------|:---:|
| Battery LED (Red) | 2 |
| Battery LED (Green) | 3 |
| Status LED | 13 |
| Battery ADC Input | A7 |
| Button ADC Input | A6 |

---

## 🎯 Core API Reference

### Initialization & Main Loop

```cpp
void init()
```
Initialize serial (115200 baud), timer, and shield hardware (if `#define SYSTEM` is set).
Call once in `setup()`.

```cpp
void process()
```
Handle incoming serial commands and update timing. With `#define SYSTEM`, also manages LEDs and monitors battery.
Call every iteration in `loop()`.

### Data Management

```cpp
bool setChannelValue(uint8_t channel, float value)
```
Set measurement value for channel (1–8).
Typically called within `setSendingCallback()`.

```cpp
float getChannelValue(uint8_t channel)
```
Retrieve current channel value.

```cpp
bool isMeasurementInProgress()
```
Check if measurement is currently active.

### Measurement Control

```cpp
void startMeasurement(bool restart = 0)
```
Start new measurement or resume paused measurement. Set `restart=1` to continue from pause.

```cpp
void stopMeasurement(bool pause = 0)
```
Stop or pause measurement. Set `pause=1` to allow resuming later.

### Callbacks

```cpp
void setSendingCallback(void (*function)(void))
```
Define function to update channel values when data is needed.
**Most important callback** — called periodically or on-demand based on sampling mode.

```cpp
void setMeasurementStartedCallback(void (*function)(void))
void setMeasurementStoppedCallback(void (*function)(void))
void setMeasurementPausedCallback(void (*function)(void))
void setMeasurementContinuedCallback(void (*function)(void))
```
Optional callbacks for measurement state transitions.

### Shield-Specific Functions (with `#define SYSTEM`)

```cpp
bool getButton(uint8_t index)  // index: 1–4 (four buttons on shield)
uint8_t getBattery()           // return: 0=empty, 1=low, 2=ok
void setPullup(uint8_t index, bool pull)  // Enable/disable pull-up resistor
```

---

## 📂 Example Sketches

Located in `examples/` directory:

| Example | Description |
|:--------|:------------|
| `analog_read` | Read analog sensor and display on graph |
| `basic_u01` | Basic sensor reading with MoleGraph Shield U01 |
| `mix_read_button_battery` | Read sensor with button and battery monitoring |
| `mix_read_led_piezo` | Combined analog read, LED control, and sound output |
| `multi_read` | Multiple sensors on different channels |
| `servo` | Control servo with analog sensor input |
| `servo2` | Advanced servo control with multiple channels |

---

## ⚙️ Communication Protocol

### Serial Settings
- **Baud rate**: 115200
- **Data bits**: 8
- **Stop bits**: 1
- **Parity**: None

### Protocol Version
- **ATG_4** (Arduino-to-Graph protocol v4)
- Compatible with MoleGraph desktop/mobile applications

### Supported Commands
The microcontroller receives and responds to:
- `INS_GET_VERSION` — Report protocol version
- `INS_START` / `INS_STOP` — Start/stop measurement
- `INS_SET_FREQUENCY` — Set sampling rate
- `INS_GET_SAMPLE` — Request immediate sample (on-demand mode)
- ... (see `enum Instructions` in `molegraphmanual.h`)

---

## ⏱️ Timer Resolution

**MoleGraphManual** uses a high-resolution timer with:
- **Time base**: 0.5 μs per tick
- **Overflow**: ~36 minutes
- **Utility functions** (in `timer.h`):
  - `getTime()` — Get current time in ticks
  - `Millis()` — Get time in milliseconds
  - `delay_us(x)` — Microsecond-level delay

---

## 🐛 Debugging

Enable debug output in your sketch:

```cpp
#define DEBUG  // Add before #include
#include <molegraphmanual.h>
```

Debug messages are sent via serial using the ATG protocol.
View in Serial Monitor or MoleGraph desktop application.

---

## 🔗 Advanced Features

### Multiple Channels
```cpp
void updateGraphChannels(void) {
  moleGraph.setChannelValue(1, analogRead(PORT_1A));
  moleGraph.setChannelValue(2, analogRead(PORT_2A));
  moleGraph.setChannelValue(3, analogRead(PORT_3A));
  moleGraph.setChannelValue(4, digitalRead(PORT_1D));
}
```

### Sampling Modes
- **Periodical**: Continuous sampling at fixed interval (default)
- **On-Demand**: Single sample when requested

### Measurement State Callbacks
```cpp
void onMeasurementStarted(void) {
  Serial.println("Measurement started!");
}

void setup() {
  moleGraph.init();
  moleGraph.setSendingCallback(&updateGraphChannels);
  moleGraph.setMeasurementStartedCallback(&onMeasurementStarted);
}
```

---

## 📖 Additional Resources

- **MoleGraph Project**: https://www.molegraph.eu
- **GitHub Repository**: https://github.com/e-Mole/MoleGraph
- **Blockly@rduino**: https://github.com/e-Mole/Arduino
- **Desktop Application**: Cross-platform (Windows, macOS, Linux, Android)

---

## 📝 License

Part of the **MoleGraph** open-source school probeware system.

---

## ❓ FAQ

**Q: Can I use MoleGraphManual without the shield?**  
A: Yes! All functionality works with raw Arduino pins. Simply omit `#define SYSTEM`. The shield adds battery monitoring, LEDs, and buttons.

**Q: What's the maximum sampling frequency?**  
A: Approximately 1 kHz for a single analog channel via USB. Bluetooth is slightly slower. Actual rate depends on serial bandwidth and desktop app processing.

**Q: Can I combine MoleGraphManual with other Arduino libraries?**  
A: Yes! MoleGraphManual is designed to coexist with other libraries (I2C sensors, displays, etc.). Just call `moleGraph.process()` regularly in your main loop.

**Q: How do I add a new sensor type?**  
A: Use the sensor's dedicated library, read data in your `updateGraphChannels()` callback, and call `setChannelValue()` with the result. MoleGraphManual handles the rest.

**Q: What's the difference between #define SYSTEM and without it?**  
A: With `#define SYSTEM`, the library enables and manages MoleGraph Shield U01 hardware (battery LEDs, status LED, buttons). Without it, you work with raw Arduino pins and must manage your own I/O.

---

**Last Updated**: 2026  
**Protocol Version**: ATG_4  
**Library Version**: See molegraphmanual.h
