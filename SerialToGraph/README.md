# SerialToGraph - MoleGraph Desktop Application

Cross-platform desktop application for MoleGraph measurements visualization, data analysis, sensor calibration, and export.

## 📚 Project Overview

**SerialToGraph** is the primary user-facing application for MoleGraph. It provides:
- ✅ Real-time measurement visualization with multi-channel graphs
- ✅ Independent Y-axes for different measurement units
- ✅ Sensor calibration interface (linear correction: y = bx + a)
- ✅ Data analytics (min, max, mean, median, standard deviation)
- ✅ Experiment templates ("Save/Open without values" for teachers)
- ✅ Export to CSV (tabular) and PNG (graphs)
- ✅ Cross-platform support (Windows, Linux, macOS, Android)
- ✅ Multi-language user interface: English, Czech, German, French, Spanish, Polish, Portuguese, Italian, and Hungarian
- ✅ Language selection directly in the application settings

Built with **Qt Framework** for native performance and consistency across platforms.

---

## 🔧 Build Requirements

### Windows
- **Qt**: 5.15.2 (prebuilt binaries from [Qt Downloads](https://download.qt.io/new_archive/qt/5.15/5.15.2/))
- **Compiler**: MinGW (bundled with Qt) or Visual Studio 2015+
- **Tools**: Git, CMake (optional)

### macOS
- **Qt**: 5.15.2 (available via [Qt Downloads](https://download.qt.io/new_archive/qt/5.15/5.15.2/) or Homebrew)
- **Compiler**: Clang (via Xcode Command Line Tools)
- **Tools**: Xcode, Git

### Linux
- **Qt**: 5.15.2 (via package manager or source)
- **Compiler**: GCC or Clang
- **Tools**: Build-essential, Git

### Android
- **Qt for Android**: 5.15.2
- **Android SDK & NDK**: Installed and configured in Qt Creator
- **Java**: JDK 8 or newer

**Note:** Qt 6.x is not supported at this time due to compatibility issues.

---

## 🚀 Build Instructions

### Desktop (Windows / Linux)

1. **Install Qt Creator** from [qt.io](https://www.qt.io/download-qt-installer)
   - Select Qt 5.15.2 during installation
   - Select your platform's compiler

2. **Clone or open the repository**
   ```bash
   git clone https://github.com/e-Mole/MoleGraph.git
   cd MoleGraph
   ```

3. **Open project in Qt Creator**
   - File → Open File or Project
   - Navigate to `SerialToGraph/SerialToGraph.pro`
   - Select your kit (compiler + Qt 5.15.2)

4. **Build in Release mode**
   - Project → Build Settings → Release (switch from Debug if needed)
   - Build → Build Project (or press Ctrl+B)

5. **Run**
   - Build → Run (or press Ctrl+R)

### macOS

**Qt version:** 5.15.2 (Qt Creator) downloaded from https://download.qt.io/new_archive/qt/5.15/5.15.2/

**Building:**

Open Qt Creator project `SerialToGraph/SerialToGraph.pro`. Set release build Project → Build Settings → Release and Build Project. Build install image file by running Qt util `macdeployqt`, see below. This step includes frameworks and libraries in the application.

```bash
~/Qt5.15.2/5.15.2/clang_64/bin/macdeployqt <path to MoleGraph.app> -verbose=2 -dmg
```

**Installation:**

Double click on `MoleGraph.dmg` to mount image file and then install it by copying `MoleGraph.app` to your Applications folder.

### Android

1. **Configure Android Kit in Qt Creator**
   - Tools → Options → Devices → Android
   - Set Android SDK and NDK paths

2. **Build for Android**
   - Select Android kit
   - Build → Build Project
   - Creates APK in build output directory

3. **Deploy to device**
   - Connect Android device via USB
   - Build → Run (installs and launches on device)

---

## 🔌 Arduino Communication

### Protocol
- **Version**: ATG_4 (Arduino-to-Graph protocol v4) or ATG_5
- **Baud Rate**: 115200
- **Data Format**: Binary packets with instruction codes

### Serial Port Detection
- Application automatically scans available COM/serial ports
- Port dialog displays connected devices
- Firmware version check on connection via `INS_GET_VERSION`

### Connection Flow
1. User selects port from PortListDialog
2. HwChannel opens serial connection
3. Firmware reports version and available channels
4. User configures sensors and sampling parameters
5. Measurement data received continuously or on-demand

---

## 🐛 Troubleshooting

### Build Error: "Qt not found" or "qmake not in PATH"
- Verify Qt 5.15.2 installation path
- In Qt Creator: Project → Build Settings → Kit → verify Qt 5.15.2 selected
- Restart Qt Creator after installing Qt

### Build Error: "Cannot find -lqcustomplot"
- Ensure `qcustomplot/` directory exists in SerialToGraph
- If missing: `git submodule update --init --recursive`

### Build Error on macOS: "Clang not found"
- Install Xcode Command Line Tools: `xcode-select --install`
- Restart Qt Creator

### Serial Port Not Detected
- Check USB cable (data cable, not charge-only)
- Verify Arduino CH340 drivers installed (clone Nano boards)
- On Linux: User may need to be in `dialout` group: `sudo usermod -aG dialout $USER`
- Restart application

### Android Build Issues
- Verify Android SDK/NDK paths in Qt Creator settings
- Check that selected Android kit matches installed Qt for Android 5.15.2
- Clean build: Projects → Run qmake, then Build → Clean All, Build

### Application Won't Launch After Build
- Run in debug mode first (F5) to see error messages
- Check console output in Qt Creator
- On macOS, verify `macdeployqt` was run to bundle dependencies

---

## 📖 Additional Resources

- **Qt 5.15 Documentation**: https://doc.qt.io/qt-5/
- **QCustomPlot**: https://www.qcustomplot.com/
- **MoleGraph Project**: https://www.molegraph.eu
- **GitHub Repository**: https://github.com/e-Mole/MoleGraph
- **Arduino Firmware**: See `Arduino/MoleGraphAuto/` and `Arduino/MoleGraphManual/`

---

**Last Updated**: 2026  
**Qt Version**: 5.15.2 (required)  
**Supported Platforms**: Windows, macOS, Linux, Android
