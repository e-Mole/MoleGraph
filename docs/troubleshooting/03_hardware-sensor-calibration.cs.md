# Průvodce hardwarovou kalibrací senzorů (MoleGraph U01)

Ačkoliv je klasická kalibrace k dispozici v nastavení měřicího kanálu přímo v softwaru MoleGraph, některá čidla vyžadují komplexní kalibraci, kterou lze provést **pouze pomocí fyzického hardwarového (HW) tlačítka** na jednotce MoleGraph U01. 

Nejčastější příklady:
* **Siloměr:** Nulování senzoru (tára).
* **Čidlo kyslíku (O2):** Nastavení na atmosférickou referenční hodnotu (21 %).
* **Čidlo barev (TCS34725):** Nastavení referencí (dle módu čidla např.: 1. stisk = bílá reference, nebo 1. stisk = bílá reference, 2. stisk = černá reference).

---

## ⚠️ Zlaté pravidlo HW kalibrace
**Hardwarová kalibrace platí vždy pouze pro právě probíhající měření!** 
Uživatelé často dělají chybu, že kalibrují čidlo ještě před spuštěním samotného měření. Pokud měření ukončíte a spustíte nové, předchozí kalibrace přestává platit a je nutné proces kalibrace zopakovat.

---

## Postup kalibrace podle režimu záznamu

Přesný postup kalibrace závisí na tom, jaký režim záznamu jste v softwaru MoleGraph zvolili.

### Režim 1: Kontinuální záznam
V tomto režimu se data zaznamenávají automaticky podle nastavené vzorkovací frekvence. Abyste zabránili uložení nezkalibrovaných dat ihned po spuštění, postupujte takto:

1. Klikněte na tlačítko **START** v softwaru MoleGraph (tím zahájíte měření).
2. Okamžitě klikněte na tlačítko **PAUSE** v softwaru, čímž pozastavíte ukládání dat.
3. Stiskněte **HW tlačítko** na jednotce MoleGraph U01 pro provedení kalibrace.
4. Klikněte na tlačítko **CONTINUE** v softwaru. Nová kalibrace se ihned aplikuje a pokračuje korektní záznam dat.

### Režim 2: Záznam na vyžádání
V tomto režimu je po spuštění měření sice aktivní, ale hodnota se uloží vždy až po vašem výslovném kliknutí na tlačítko SAMPLE.

1. Klikněte na tlačítko **START** v softwaru MoleGraph. Měření se spustí, ale zatím se neukládají žádná data.
2. Stiskněte **HW tlačítko** na jednotce MoleGraph U01 pro provedení kalibrace.
3. Nyní můžete kliknout na tlačítko **SAMPLE** v softwaru a bezpečně uložit první (a každý další) správně zkalibrovaný bod.

---
> **Shrnutí:** Vždy nejprve spusťte měření v softwaru, následně proveďte HW kalibraci na krabičce a až poté ukládejte/zaznamenávejte data.
