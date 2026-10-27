# Hardware Sensor Calibration Guide (MoleGraph U01)

While standard calibration options are available in the measurement channel settings within the MoleGraph software, certain sensors require complex calibration routines that can **only be triggered using the physical hardware (HW) button** on the MoleGraph U01 unit. 

Common examples include:
* **Force Sensor:** Zeroing the scale (Tare).
* **Oxygen Sensor (O2):** Calibrating to the atmospheric baseline (21%).
* **Color Sensor (TCS34725):** Setting references (1st press = White reference, 2nd press = Black reference).

---

## ⚠️ The Golden Rule of HW Calibration
**Hardware calibration is only valid for the currently running measurement session!** 
Users often calibrate the sensor before starting the measurement, which is incorrect. If you stop the measurement and start a new one, the previous calibration is discarded, and you must calibrate the sensor again.

---

## Calibration Workflow by Recording Mode

The exact steps to calibrate depend on the recording mode you have selected in the MoleGraph software.

### Mode 1: Continuous Recording (Kontinuální)
In this mode, data is recorded automatically based on a set sampling frequency. To calibrate without recording skewed data:

1. Click **START** in the MoleGraph software to begin the measurement session.
2. Immediately click **PAUSE** in the software to halt data collection.
3. Press the **HW button** on the MoleGraph U01 unit to perform the calibration.
4. Click **CONTINUE** in the software. The new calibration is instantly applied, and accurate data recording resumes.

### Mode 2: On-Demand Recording (Na vyžádání)
In this mode, the session is active, but a data point is only saved when you explicitly click the SAMPLE button.

1. Click **START** in the MoleGraph software. The session is now active, but no data is being saved yet.
2. Press the **HW button** on the MoleGraph U01 unit to perform the calibration.
3. You can now click the **SAMPLE** button in the software to record your first (and subsequent) calibrated data points.

---
> **Summary:** Always start the measurement in the software *first*, then trigger the HW calibration, and finally proceed with logging your data points.