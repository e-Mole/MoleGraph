# Complete Guide to Reviving and Flashing Arduino Nano for Large Firmwares

This procedure serves as a verified guide for working with a new Arduino Nano, ensuring the proper installation of Optiboot, bypassing IDE limitations, and guaranteeing stable communication with the MoleGraph application. Due to the sensitivity of a 32 KB firmware to power supply quality, we strongly recommend using exclusively high-quality "precision" clones or original boards (cheap Chinese no-name clones skimp on decoupling capacitors and resonators, which leads to startup instability).

---

## 1. Configuration in boards.txt (One-time setup)
Modify the `boards.txt` configuration file in your Arduino IDE (located in the `hardware/arduino/avr/` folder) so that the Arduino Nano utilizes the parameters of a proven Arduino Uno (Optiboot, maximum firmware memory capacity of 32256 bytes):

nano.name=Arduino Nano

nano.upload.tool=avrdude
nano.upload.protocol=arduino

nano.bootloader.tool=avrdude
nano.bootloader.unlock_bits=0x3F
nano.bootloader.lock_bits=0x0F

nano.build.f_cpu=16000000L
nano.build.board=AVR_NANO
nano.build.core=arduino
nano.build.variant=eightanaloginputs

## Arduino Nano w/ ATmega328P
## --------------------------
nano.menu.cpu.atmega328=ATmega328P

nano.menu.cpu.atmega328.upload.maximum_size=32256
nano.menu.cpu.atmega328.upload.maximum_data_size=2048
nano.menu.cpu.atmega328.upload.speed=115200

nano.menu.cpu.atmega328.bootloader.low_fuses=0xFF
nano.menu.cpu.atmega328.bootloader.high_fuses=0xDE
nano.menu.cpu.atmega328.bootloader.extended_fuses=0xFD
nano.menu.cpu.atmega328.bootloader.file=optiboot/optiboot_atmega328.hex

nano.menu.cpu.atmega328.build.mcu=atmega328p

---

## 2. Compilation in Arduino IDE
1. In the `config.h` file of your project, select sensors so that the resulting compiled code size is safely below 30,720 bytes (e.g., temporarily disable resource-heavy modules like accelerometers to address firmwares larger than 30720 bytes only after verifying a stable foundation).
2. In the Arduino IDE, select the **Arduino Nano** board (with the ATmega328P processor).
3. Compile the code (`Export compiled Binary`) to generate the `_with_bootloader.hex` file.

---

## 3. Connecting to the ISP Programmer (Arduino as ISP)
1. Take a second, fully functional Arduino and upload the **ArduinoISP** example sketch to it.
2. Connect the programmer to the new target Nano via the SPI bus:
   * 5V -> 5V, GND -> GND
   * Pin 10 (programmer) -> RESET (Nano)
   * Pin 11 (programmer) -> MOSI (Nano / pin 11)
   * Pin 12 (programmer) -> MISO (Nano / pin 12)
   * Pin 13 (programmer) -> SCK (Nano / pin 13)

---

## 4. AVRDUDESS and Verification of Fuses & Lock bits
1. Launch AVRDUDESS, select the `Arduino as ISP` programmer and the `ATmega328P` MCU.
2. Check and write the correct fuse values:
   * **Low Fuse (L):** `0xFF`
   * **High Fuse (H):** `0xDE` (enables 500B bootloader and BOD at 2.7V)
   * **Extended Fuse (E):** `0xFD`
   * **Lock bits (LB):** `0xFF`
3. Click **Write** next to Fuses.

---

## 5. ISP Flashing of the `_with_bootloader.hex` File
1. In the Flash section of AVRDUDESS, select the generated `project.ino.with_bootloader.hex` file.
2. Click **Program** for direct writing via ISP (including Optiboot).
3. Upon completion, disconnect the ISP wiring from the Arduino Nano.

---

## 6. Functional Verification in MoleGraph
1. Connect the newly revived Nano to your computer via a USB cable.
2. Launch the **MoleGraph** application and verify that connection establishment and data transfer proceed successfully.

---

## 7. Standard Over-the-USB Upload
1. Close/disconnect MoleGraph.
2. Open the Arduino IDE and attempt to upload code to this Nano **via standard USB** (by clicking the Upload arrow).
3. Verify that the upload completes and the board boots up successfully after a reset.
4. You can now experiment with required sensors in `config.h`—enable all necessary sensors and try uploading the firmware to the Arduino normally from the Arduino IDE (as long as it fits within 32256 bytes, the upload should complete without issues).

---

## 8. Final Verification and Troubleshooting
1. Re-test the connection in the **MoleGraph** application. If everything works, this Nano has a stable foundation and is ready for further use.
2. **If it fails (lit RX LED, unresponsive, line occupied...):**
   * **Disconnect ArduinoISP from USB:** Reconnect the cable and test the connection from MoleGraph again.
   * **Restart MoleGraph:** Close the MoleGraph application, reopen it, and test the connection again.
   * **Change the USB port:** Move the cable to a different USB port on the computer (ideally directly on the motherboard's rear panel; avoid front case panels and cheap USB hubs).
   * **Reduce the code size:** If changing ports does not help, return to `config.h`, disable an unused sensor (such as the aforementioned accelerometer) to shrink the code slightly, and repeat the entire procedure until 100% repeatable stability is achieved.