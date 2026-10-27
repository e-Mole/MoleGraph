# Kompletní manuál oživení a flashování Arduino Nano pro velké firmwary

Tento postup slouží jako ověřený návod pro práci s novým Arduino Nano, zajišťuje správnou instalaci Optibootu, obchází limity IDE a garantuje stabilitu komunikace s aplikací MoleGraph. Vzhledem k citlivosti 32KB firmware na napájení doporučujeme používat výhradně kvalitní "precizní" klony nebo originální desky (levné čínské no-name klony šetří na blokovacích kondenzátorech a rezonátorech, což vede k nestabilitě startu).

---

## 1. Nastavení v souboru boards.txt (Jednorázově)
Upravte konfigurační soubor `boards.txt` vašeho Arduino IDE (ve složce `hardware/arduino/avr/`), aby Arduino Nano využívalo parametry prověřeného Arduina Uno (Optiboot, kapacita paměti pro FW 32256 bajtů):

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

## 2. Kompilace v Arduino IDE
1. V `config.h` vašeho projektu vyberte senzory tak, aby výsledná velikost zkompilovaného kódu byla bezpečně pod 30 720 bajtů (např. dočasně vypněte náročné moduly typu akcelerometr, abychom řešili FW větší než 30720 b až po ověření stabilního základu).
2. V Arduino IDE vyberte desku **Arduino Nano** (s procesorem ATmega328P).
3. Proveďte kompilaci kódu (`Exportovat kompilační binární soubor` / Export compiled Binary), abyste získali soubor `_with_bootloader.hex`.

---

## 3. Připojení k ISP programátoru (Arduino jako ISP)
1. Vezměte druhé, plně funkční Arduino, nahrajte do něj příklad **ArduinoISP**.
2. Propojte programátor s novým cílovým Nanem přes SPI sběrnici:
   * 5V -> 5V, GND -> GND
   * Pin 10 (prog) -> RESET (Nano)
   * Pin 11 (prog) -> MOSI (Nano / pin 11)
   * Pin 12 (prog) -> MISO (Nano / pin 12)
   * Pin 13 (prog) -> SCK (Nano / pin 13)

---

## 4. AVRDUDESS a kontrola Fuses & Lock bits
1. Spusťte AVRDUDESS, zvolte programátor `Arduino as ISP` a MCU `ATmega328P`.
2. Zkontrolujte a zapište správné hodnoty pojistek:
   * **Low Fuse (L):** `0xFF`
   * **High Fuse (H):** `0xDE` (zapíná 500B bootloader a BOD na 2.7V)
   * **Extended Fuse (E):** `0xFD`
   * **Lock bits (LB):** `0xFF`
3. Klikněte na **Write** u Fuses.

---

## 5. ISP flash souboru `_with_bootloader.hex`
1. V sekci Flash v AVRDUDESS vyberte vygenerovaný soubor `projekt.ino.with_bootloader.hex`.
2. Klikněte na **Program** pro přímý zápis přes ISP (včetně Optibootu).
3. Po dokončení odpojte ISP kabeláž od Arduina Nano.

---

## 6. Ověření funkčnosti v programu MoleGraph
1. Připojte nově oživené Nano k počítači přes USB kabel.
2. Spusťte program **MoleGraph** a ověřte, že proběhne úspěšné navázání komunikace a přenos dat.

---

## 7. Klasické přehrání přes USB
1. Ukončete/odpojte MoleGraph.
2. Otevřete Arduino IDE a zkuste do tohoto Nano nahrát kód **standardní cestou přes USB** (kliknutím na šipku Nahrát).
3. Ověřte, že upload proběhne a deska po resetu bez problémů naskočí.
4. Nyní můžete experimentovat s potřebnými čidly (soubor config.h) - zapněte všechna čidla, která potřebujete a zkuste FW nahrát do Arduina klasicky z Arduino IDE (pokud se vejdete do 32256 B, mělo by nahrání proběhnout bez problémů).  

---

## 8. Finální ověření a řešení případných problémů
1. Znovu otestujte připojení v programu **MoleGraph**. Pokud vše funguje, má toto Nano stabilní základ a můžete s ním dále pracovat.
2. **Když to selže (svítící RX, neodpovídá, linka je obsazená...):**
   * **Odpojte ArduinoISP od USB:** Připojte kabel a otestujte připojení z MoleGraph znovu.
   * **Vypněte aplikaci MoleGraph a znovu ji spusťte:** Otestujte připojení z MoleGraph znovu.
   * **Změňte USB port:** Přepojte kabel do jiného portu počítače (ideálně přímo do zadního panelu základní desky, vyhněte se předním panelům a levným USB hubům).
   * **Zmenšete kód:** Pokud port nepomůže, vraťte se do `config.h`, vypněte některé čidlo, které nepoužíváte (např. zmíněný akcelerometr), čímž kód ještě o kousek zmenšíte, a celý postup opakujte, dokud nedosáhnete 100% opakovatelné stability.