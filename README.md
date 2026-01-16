# MoleGraph: Open-Source School Probeware & STEM Toolkit

[![Website](https://img.shields.io/badge/website-molegraph.eu-blue)](https://www.molegraph.eu)
![License](https://img.shields.io/badge/project-Open%20Source-green)
![Platform](https://img.shields.io/badge/platform-Windows%20|%20Android%20|%20Linux%20|%20macOS-blue)
![Hardware](https://img.shields.io/badge/hardware-Arduino%20|%20DIY%20|%203D%20Print-orange)

**MoleGraph** is an open-source, low-cost school probeware system designed for **hands-on STEM education**. Unlike commercial "black box" solutions, it follows a **"white box" approach**, allowing students and teachers to understand the inner workings of measurement technology, electronics, and sensor calibration.

By engaging in the **DIY (Do-It-Yourself) assembly process**—which includes soldering, 3D printing, and component sourcing—users develop deep technological literacy and critical thinking skills.

---
<img src="http://www.e-mole.cz/sites/default/files/pictures/molegraph-release.jpg" alt="MoleGraph in action" />

## 🏗️ Hardware Architecture & Modular Construction
The system is built on the **Arduino platform**, specifically utilizing the **Arduino NANO** (ATmega328) microcontroller and an **HC-05 Bluetooth module** for wireless connectivity.

* **[MoleGraph Shield U01](https://www.e-mole.cz/diy/molegraph-shield):** A custom-designed expansion PCB that features **standard RJ12 connectors** for robust and reliable sensor connectivity, replacing unstable breadboard wires.
* **Mechanical Compatibility:** All components are housed in **[3D-printed cases (MoleBoxes)](https://www.e-mole.cz/diy/molebox)** that are fully compatible with the **[m-BITBEAM](https://www.e-mole.cz/diy/m-bitbeam) and BITBEAM** open-source construction sets.
* **LEGO Integration:** These cases share dimensions and hole spacing with **LEGO Technic**, allowing sensors to be integrated directly into robotic and mechanical models.

---

## ⚙️ Hardware Parameters (MoleGraph Unit U01)

| Feature | Specification |
| :--- | :--- |
| **Sensor Ports** | 4× **RJ12 ports**, each equipped with a **programmable function button** (e.g., for sensor resetting or triggering). |
| **Actuators** | Ports 2 and 3 natively support the connection of **servo motors**, enabling automation and robotics tasks. |
| **Status Indicators** | Onboard LEDs for **battery status**, active **Bluetooth/USB** connection, and ongoing **data recording**. |
| **Power Supply** | Flexible power via a **9V battery (6LR61)** or a **USB connection**. |
| **Connectivity** | Dual communication modes supporting **USB cable** (serial-to-USB) and wireless **Bluetooth SPP**. |
| **Analog Sensors** | 0–5V range using the integrated **10-bit ADC**. |
| **Digital Sensors** | Supports protocols like **I²C, OneWire, and PWM**. |
| **Extensibility** | Currently supports **30+ sensor types**. Includes "universal cables" and breadboard modules for connecting any custom 0–5V sensor. |

---

## 🖥️ Software & Data Analysis
The system includes a powerful cross-platform application and a specialized firmware layer.

| Feature | Details |
| :--- | :--- |
| **Platforms** | Native support for **MS Windows, Linux, macOS, and Android**. |
| **Measurement** | Manages up to **8 independent channels** simultaneously. |
| **Sampling** | Continuous sampling (up to **~1 kHz** for one analog) or **on-demand sampling** triggered manually. |
| **Visualization** | Real-time **graphs** (with multiple independent Y-axes) and **digital panels** for numerical display. |
| **Analytics** | Graph reading tools and basic **statistics** (min, max, mean, median, standard deviation). |
| **Calibration** | Built-in calibration/correction dialog using formula, e.g. $y = bx + a$. |
| **Education** | Teachers can create experiment templates ("Save/Open without values"), allowing students to start measuring immediately. |
| **Export** | Export results to **CSV** (tabular data) and graphs to **PNG** images. |

---

## 💻 Programming & Robotics Integration
MoleGraph facilitates a smooth transition from basic measurements to advanced coding and engineering.

### 1. Visual Programming
Beginners can use **[Blockly@rduino](https://github.com/e-Mole/Arduino)** with a custom MoleGraph extension (and **`MoleGraphManual` library**) to build programs graphically, automatically generating standard Arduino (C++) code.

### 2. Advanced Coding
The **`MoleGraphManual` library** allows for custom firmware development in the Arduino IDE, enabling simultaneous data transmission and control of external actuators.

### 3. Robotics
Highly compatible with projects like the [**"Fretka" robot**](https://www.e-mole.cz/clanek/bezdratova-komunikace-pro-fretku), which can transmit real-time sensor data wirelessly to the MoleGraph application.


## Different Arduino and MoleGraph setups

<img src="http://www.e-mole.cz/sites/default/files/pictures/molegraph-var_0.jpg" alt="MoleGraph in action" />

## MoleGraph shield

<img src="http://www.e-mole.cz/sites/default/files/pictures/molegraph-shield_0.jpg" alt="MoleGraph shield" />

## Desktop app for Mac OS X building instructions

### QT version
QT version is 5.10.1 (QT Creator 4.5.1) downloaded from https://download.qt.io/new_archive/qt/5.10/5.10.1/.

### Building
Open Qt Creator project SerialToGraph/SerialToGraph.pro. Set release build Project->Build Settings->Realease and Build Project. Build install image file by run Qt util macdeployqt, see below. This step includes frameworks and libraries in the application.

`~/Qt5.10.1/5.10.1/clang_64/bin/macdeployqt <path to MoleGraph.app> -verbose=2 -dmg`

### Installation
Double click on MoleGraph.dmg to mount image file and than install it by copying MoleGraph.app to your Application folder.
