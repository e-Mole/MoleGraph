# Optimalizace paměti: Zapínání a vypínání senzorů v config.h

Arduino Nano má přísný limit na velikost paměti pro firmware (cca 30–32 KB v závislosti na bootloaderu). Fyzicky není možné nahrát do desky firmware MoleGraph, ve kterém by byla *současně zapnuta úplně všechna dostupná čidla*. 

Řešením je vypnout (deaktivovat) ta čidla, která nepoužíváte. To se dělá v centrálním konfiguračním souboru s názvem `config.h`.

---

## Návod krok za krokem

### Krok 1: Otevření projektu v Arduino IDE
1. Spusťte program Arduino IDE.
2. Otevřete hlavní soubor s firmwarem MoleGraph (`MoleGraphAuto.ino`).
3. Podívejte se na horní lištu nad editorem kódu – uvidíte tam několik záložek. Klikněte na záložku pojmenovanou **`config.h`**.

### Krok 2: Jak to funguje (Kouzlo znaků `//`)
V tomto souboru zapínáte nebo vypínáte senzory tím, že na začátek řádku přidáte, nebo naopak smažete dvě lomítka (`//`). Tomuto postupu se říká "zakomentování".

* **ZAPNUTO (Aktivní):** Bez lomítek. Kompilátor řádek přečte a kód senzoru zabere paměť Arduina.
  `#define ENABLE_FORCE`
* **VYPNUTO (Neaktivní):** Začíná na `//`. Kompilátor řádek ignoruje, čímž ušetříte cennou paměť!
  `//#define ENABLE_FORCE`

### Krok 3: Vypnutí nepoužívaných čidel (Příklad: Akcelerometr)
Komplexní čidla, jako je akcelerometr LSM303DLHC, vyžadují složité matematické výpočty a zabírají obrovské množství paměti. Pokud s ním neměříte, vypněte ho!

Sjeďte v kódu níže do sekce `--- 2. MECHANICS & PHYSICS ---` a najděte řádek s akcelerometrem.

**Předtím (Zapnuto - zabírá paměť):**
```cpp
#define ENABLE_LSM303DLHC       // I003 Akcelerometr + magnetometr LSM303DLHC
```

**Potom (Vypnuto - paměť ušetřena):**
```cpp
//#define ENABLE_LSM303DLHC       // I003 Akcelerometr + magnetometr LSM303DLHC
```
*Tip: Prostě jen napište `//` na úplný začátek daného řádku.*

### Krok 4: Kompilace a nahrání do desky
1. Projděte si celý soubor `config.h` a vypněte (`//`) všechna čidla, která s jednotkou MoleGraph nepoužíváte.
2. Nechte zapnutá (bez `//`) pouze ta čidla, se kterými běžně pracujete.
3. Klikněte na tlačítko **Ověřit** (ikonka "fajfky" vlevo nahoře v Arduino IDE), abyste se ujistili, že se výsledný kód vejde do paměti.
4. Klikněte na tlačítko **Nahrát** (ikonka šipky) a nahrajte tento optimalizovaný firmware do vašeho Arduina Nano.

> **Dobrá praxe:** Udržujte svůj firmware co nejmenší. Menší kód běží rychleji, je stabilnější a máte 100% jistotu, že nenarazíte na hardwarové limity paměti Arduina.
