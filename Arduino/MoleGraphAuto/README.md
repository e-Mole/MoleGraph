# MoleGraphAuto - Arduino Firmware

Open-source firmware for **MoleGraph** school probeware system with 30+ built-in sensor support and automatic measurement synchronization.

## 📚 Overview

**MoleGraphAuto** is the recommended firmware for most MoleGraph installations. It provides:
- ✅ Automatic support for 30+ sensor types (temperature, pressure, distance, acceleration, pH, CO₂, oxygen, heart rate, color, UV, etc.)
- ✅ Multi-channel simultaneous sampling (up to 8 independent channels)
- ✅ Two sampling modes: Periodical (continuous) & On-Demand
- ✅ Support for I2C, OneWire, PWM, and analog (0-5V) sensor protocols
- ✅ Servo motor control on ports 2 & 3
- ✅ Built-in sensor calibration via desktop application
- ✅ Real-time data transmission to desktop/mobile applications
- ✅ Easy sensor configuration without code editing

### When to Use

| Use **MoleGraphAuto** | Use **MoleGraphManual** |
|:---|:---|
| Standard measurements with built-in sensors | Custom firmware with simultaneous data transmission |
| Classroom experiments & STEM education | Actuator control (servos, motors, etc.) |
| Automatic sensor calibration via app | Advanced experimental designs |
| Plug-and-play measurements | Visual programming (Blockly@rduino) |

---

## 🔧 Hardware Requirements

### Microcontroller
- **Arduino NANO** (ATmega328P) — primary platform
- 2 KB RAM + 32 KB Flash memory total
- Up to 30 KB available for firmware (sensors consume ~24-28 KB depending on config)

### MoleGraph Shield U01
- 4x RJ12 sensor ports (mandatory for most sensors)
- Battery voltage monitoring LEDs (red = low, green = ok)
- Status indicator LED
- Button input with 4 selectable levels
- Dedicated pins for servo control and power management

### Connectivity
- **USB Serial** (CH340 or FT232 chip) — 115200 baud
- **Bluetooth HC-05 module** (optional) — wireless communication
- **Note:** Arduino NANO has only one serial connection; operates in exclusive mode (USB **or** Bluetooth, not simultaneously)

### Supported Sensors
30+ sensor types organized by category:
- **Kinematics & Distance**: VL53L0X/VL53L1X laser distance, SRF04 ultrasonic, digital caliper, photogate
- **Mechanics & Physics**: Force gauge (HX711), pressure (MPX5700DP), accelerometer, magnetometer, sound intensity
- **Thermodynamics**: DS18B20, MLX90614 IR thermometer, MAX6675 thermocouple, BME280 (pressure+temp+humidity)
- **Light & Optics**: Lux sensor, BH1750, UV radiation (VEML6070), color sensor (TCS34725)
- **Chemistry**: pH, conductivity/salinity, ORP, turbidity, CO₂ (MH-Z16), O₂ (ME2-O2), gas sensors (MQ-2, MQ-3)
- **Biology & Medical**: Heart rate, ECG (AD8232)
- **Electricity**: Voltage (0-25V), current (0-5A, 0-30A)

---

## 💾 Installation

### Option 1: Arduino IDE (Recommended)
1. Download the latest MoleGraphAuto firmware from [GitHub](https://github.com/e-Mole/MoleGraph)
2. Open `Arduino/MoleGraphAuto/MoleGraphAuto.ino` in Arduino IDE
3. Select: **Board** → Arduino NANO, **Processor** → ATmega328P
4. Click **Upload**

### Option 2: Pre-compiled Hex File (Advanced)
Use a hex programmer to upload pre-compiled firmware directly to the Arduino.

---

## ⚙️ Configuration (config.h)

### Memory Management

Arduino NANO has limited memory. If compilation fails with "memory exceeded" error, you must disable unused sensors.

**File location:** `Arduino/MoleGraphAuto/config.h`

**How to disable sensors:**
1. Open `config.h` in a text editor
2. Find the sensor you don't need
3. Add `//` at the beginning of the line to comment it out
4. Save and recompile

**Example:**
```cpp
#define ENABLE_DS18B20          // Temperature sensor - ENABLED
//#define ENABLE_SRF04          // Ultrasonic sensor - DISABLED (commented out)
#define ENABLE_BME280           // Barometer - ENABLED
```

**Recommended for memory optimization:**
- Disable sensors you don't use in your lessons
- Start by disabling less common sensors (Geiger counter, spirometer, etc.)
- Chemistry sensors can also be disabled if not needed

### Color Sensor Configuration (TCS34725)

The color sensor has two firmware profiles optimized for different measurement scenarios.

**File location:** `Arduino/MoleGraphAuto/config.h` (lines 51-72)

#### PROFILE_BASIC (Default - Lightweight)
```cpp
#define PROFILE_BASIC 1           // ENABLED
//#define PROFILE_MONITOR 1       // DISABLED
```
- Lightweight, uses less memory (~2 KB)
- Single-click: White calibration only
- Double-click: Toggle LED on/off + undo calibration
- **Use for:** Reflective surfaces, paper, general color measurements

#### PROFILE_MONITOR (Advanced - For Emissive Screens)
```cpp
//#define PROFILE_BASIC 1         // DISABLED
#define PROFILE_MONITOR 1         // ENABLED
```
- Advanced color processing with LUT (lookup tables) and sRGB gamma correction
- Requires additional memory (~4 KB) — may need to disable other sensors
- Two-step calibration:
  - Step 1: Single-click on white area of screen (sets white point)
  - Step 2: Single-click on black area within 10 seconds (sets black point for backlight compensation)
- Double-click: Toggle LED + undo calibration
- **Use for:** Light-emitting displays, monitors, screens requiring precise color measurement

**Switching profiles:**
If you need PROFILE_MONITOR but get "memory exceeded" error:
1. Keep only essential sensors enabled in config.h
2. Disable chemistry sensors, gas sensors, or other non-essential measurements
3. Recompile

### Distance Sensor Configuration (VL53L0X/VL53L1X)

Two hardware versions available with different range and features.

**File location:** `Arduino/MoleGraphAuto/config.h` (lines 18-19)

#### VL53L0X (Original - 1.2 m range)
```cpp
#define ENABLE_VL53L0X          // ENABLED
    //#define USE_VL53L1X        // DISABLED
```
- Range: Up to 1.2 meters
- Lower cost, older sensor
- **Use for:** Close-range measurements, standard classroom experiments
- Default setting

#### VL53L1X (New - 4 m range)
```cpp
#define ENABLE_VL53L0X          // ENABLED
    #define USE_VL53L1X         // ENABLED
```
- Range: Up to 4 meters
- Higher accuracy, extended range
- **Use for:** Long-distance measurements, larger spaces
- Requires same memory footprint as VL53L0X

**How to switch:**
Simply comment/uncomment the `#define USE_VL53L1X` line on line 19.

---

## 🚀 First Steps

1. **Flash the firmware** to Arduino NANO (see Installation above)
2. **Connect MoleGraph Shield U01** to Arduino
3. **Attach a sensor** to one of the 4 RJ12 ports
4. **Connect to desktop app** via USB or Bluetooth
5. **Select sensor type** in the app (auto-detected if configured correctly)
6. **Start measuring**

---

## 🔧 Troubleshooting

### Compilation Error: "Memory Exceeded"
**Solution:** Disable unused sensors in `config.h`
```cpp
//#define ENABLE_MQ2              // Comment out unused gas sensor
//#define ENABLE_ORP              // Comment out unused chemistry sensor
```

### Arduino IDE Shows "Board Not Found"
**Solution:** 
1. Check USB cable (data cable, not charge-only)
2. Install CH340 drivers if using clone Arduino NANO
3. Select Board: Arduino NANO, Processor: ATmega328P

### Sensor Not Detected
1. Check physical RJ12 connection
2. In `config.h`, verify the sensor's `#define ENABLE_...` is NOT commented out
3. Recompile and upload firmware
4. Restart desktop app and re-scan ports

### Color Sensor Calibration Issues
- **PROFILE_BASIC:** Single-click on white surface (paper, wall) to calibrate
- **PROFILE_MONITOR:** 
  - First click on white screen area
  - Second click on black screen area within 10 seconds
  - If timeout, starts over at Step 1

### Out of Memory with Color Sensor Advanced Mode
- Switch to `PROFILE_BASIC` instead of `PROFILE_MONITOR`
- Or disable non-essential sensors (MQ-2, MQ-3, ORP, etc.)

---

## 📌 PIN Assignments

### Shield U01 Ports (4 Sensor Connections)

| Port | Analog Pin | Digital Pin | Pull-up Pin |
|:----:|:----------:|:-----------:|:-----------:|
|  1   |     14     |      11     |      7      |
|  2   |     15     |      10     |      6      |
|  3   |     16     |      9      |      5      |
|  4   |     19     |      8      |      4      |

### Special Pins

| Feature | Pin |
|:--------|:---:|
| Battery LED (Red) | 2 |
| Battery LED (Green) | 3 |
| Status LED | 13 |
| Battery ADC Input | A7 |
| Button ADC Input | A6 |
| Servo Port 1 (SG90, etc.) | 2* |
| Servo Port 2 (SG90, etc.) | 3* |

*Note: Pins 2 & 3 shared with LED outputs; use carefully or disable LEDs if needed

---

## ⚙️ Communication Protocol

### Serial Settings
- **Baud rate**: 115200
- **Data bits**: 8
- **Stop bits**: 1
- **Parity**: None

### Protocol Version
- **ATG_5** (Arduino-to-Graph protocol v5)
- Compatible with MoleGraph desktop application (Windows, macOS, Linux, Android)

### Synchronization
The firmware automatically synchronizes with the desktop app:
- Firmware receives measurement timing instructions
- Data is sent periodically or on-demand based on app settings
- Desktop app handles real-time visualization and data logging

---

## 🎓 Educational Use

**Recommended sensor combinations for common experiments:**

| Experiment | Required Sensors | Config Notes |
|:-----------|:-----------------|:------------|
| Weather Station | BME280 (temp/pressure/humidity), BH1750 (light), VEML6070 (UV) | Disable chemical sensors |
| Color Analysis | TCS34725, BH1750 | Use PROFILE_BASIC to save memory |
| Motion Study | VL53L0X, LSM303DLHC (accelerometer) | Enable both kinematics sensors |
| Plant Growth | DS18B20 (temp), BME280 (humidity), BH1750 (light) | Simple setup, low memory |
| Water Quality | pH, conductivity, turbidity, O₂ | Keep chemistry sensors only |

---

## 📖 Additional Resources

- **MoleGraph Project**: https://www.molegraph.eu
- **GitHub Repository**: https://github.com/e-Mole/MoleGraph
- **Desktop Application**: Cross-platform (Windows, macOS, Linux, Android)
- **Sensor Documentation**: See individual sensor files in Arduino/MoleGraphAuto/

---

## ❓ FAQ

**Q: How many sensors can I use simultaneously?**  
A: Up to 8 channels can be measured simultaneously on the 4 physical ports. Each port can support multiple sensors via I2C protocol.

**Q: Do I need to edit code to add/remove sensors?**  
A: No code editing required for basic use. Only edit `config.h` if you hit memory limits or need to select between sensor variants (color profile, distance sensor version).

**Q: Can I use MoleGraphAuto without the shield?**  
A: Most sensors require the shield's RJ12 connectors. Bare Arduino can only read analog values. We recommend using the shield.

**Q: What happens if I enable all sensors but run out of memory?**  
A: Arduino IDE will show a compilation error. Disable the least-used sensors in `config.h` and recompile.

**Q: Can I switch between PROFILE_BASIC and PROFILE_MONITOR?**  
A: Yes, by editing one line in `config.h` and recompiling. No hardware changes needed.

**Q: How do I recover if the Arduino won't upload?**  
A: Double-click the Reset button on Arduino NANO quickly to enter bootloader mode, then try uploading again.

---

**Last Updated**: 2026  
**Protocol Version**: ATG_5  
**Firmware Version**: 4.4  
**Supported Sensors**: 30+
