# Optimizing Memory: Enabling and Disabling Sensors in config.h

The Arduino Nano has a strict memory limit for the firmware (approx. 30–32 KB depending on the bootloader). It is physically impossible to upload the MoleGraph firmware with *every single sensor enabled* at the same time. 

To solve this, you need to turn off (disable) the sensors you are not currently using. This is done in a central configuration file called `config.h`.

---

## Step-by-Step Guide

### Step 1: Open the Project in Arduino IDE
1. Open the Arduino IDE.
2. Open the main MoleGraph firmware file (`MoleGraphAuto.ino`).
3. Look at the top bar above the code editor – you will see multiple tabs. Click on the tab named **`config.h`**.

### Step 2: Understand the Syntax (The `//` Magic)
In this file, you enable or disable sensors by adding or removing two forward slashes (`//`) at the beginning of a line. This is called "commenting."

* **ENABLED (Active):** No slashes. The compiler reads it, and it takes up Arduino memory.
  `#define ENABLE_FORCE`
* **DISABLED (Inactive):** Starts with `//`. The compiler ignores it, saving valuable memory!
  `//#define ENABLE_FORCE`

### Step 3: Disable Unused Sensors (Example: Accelerometer)
Complex sensors, like the LSM303DLHC accelerometer, require heavy mathematical calculations and consume a massive amount of memory. If you are not using it today, turn it off!

Scroll down to `--- 2. MECHANICS & PHYSICS ---` and find the accelerometer line.

**Before (Enabled - consumes memory):**
```cpp
#define ENABLE_LSM303DLHC       // I003 Accelerometer and magnetometer LSM303DLHC
```

**After (Disabled - memory saved):**
```cpp
//#define ENABLE_LSM303DLHC       // I003 Accelerometer and magnetometer LSM303DLHC
```
*Tip: Simply type `//` at the very beginning of the line.*

### Step 4: Compile and Upload
1. Go through the entire `config.h` file and disable (`//`) all sensors you don't physically have connected to your MoleGraph unit.
2. Leave only your currently used sensors enabled (without `//`).
3. Click the **Verify** (Checkmark) button in the top left corner of the Arduino IDE to ensure the compiled code fits within the memory limit.
4. Click the **Upload** (Arrow) button to flash the optimized firmware to your Arduino Nano.

> **Best Practice:** Keep your firmware as lean as possible. A smaller firmware runs faster, is more stable, and guarantees you won't hit the Arduino's hardware memory ceiling.