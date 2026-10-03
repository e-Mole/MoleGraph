#ifndef CONFIG_H
#define CONFIG_H

// =========================================================================
// MOLEGRAPH - CENTRAL CONFIGURATION SENSOR FILE
// (CENTRÁLNÍ KONFIGURAČNÍ SOUBOR ČIDEL)
// =========================================================================
// Uncomment (remove '//') to enable a sensor. 
// (Odkomentuj odstraněním '//' pro zapnutí senzoru.)
// Comment out (add '//') to disable it and save Arduino memory.
// (Zakomentuj přidáním '//' pro jeho vypnutí a úsporu paměti Arduina.)
// =========================================================================
// Note: SENSOR_AD (A101) and SENSOR_TIMER (D103) are always enabled by default.
// (Poznámka: SENSOR_AD (A101) a SENSOR_TIMER (D103) jsou ve výchozím stavu vždy zapnuty.)
// =========================================================================

// --- 1. KINEMATICS & DISTANCE (Kinematika a dálkoměry) ---
#define ENABLE_VL53L0X          // I105 Distance sensor VL53L0X / VL53L1X (Dálkoměr VL53L0X / VL53L1X)
    //#define USE_VL53L1X         // Enable (uncomment) for new 4m sensor (VL53L1X). Comment out for old 1.2m sensor (VL53L0X).
#define ENABLE_SRF04            // D021 Motion sensor returning distance, velocity, acceleration (Čidlo pohybu - vrací vzdálenost + rychlost + zrychlení)
#define ENABLE_CALIPER          // D035 Length sensor - digital caliper (Čidlo "délky" - šupléra čínská levná)
#define ENABLE_P_GATE           // A039 Photogate switch/slot 810H (Fotobrána spínač/štěrbina 810H)

// --- 2. MECHANICS & PHYSICS (Mechanika a fyzika) ---
#define ENABLE_FORCE            // D007 Force gauge with HX711 (Siloměr s HX711)
#define ENABLE_MPX5700DP        // A006 Pressure sensor MPX5700DP (Tlakoměr MPX5700DP)
#define ENABLE_LSM303DLHC       // I003 Accelerometer and magnetometer LSM303DLHC (Akcelerometr + magnetometr LSM303DLHC)
#define ENABLE_MAGNETOMETR      // A023 Magnetic field sensor (Čidlo magnetického pole)
#define ENABLE_SOUNDMETER       // A027 Sound intensity sensor - microphone (Čidlo intenzity zvuku - mikrofon)

// --- 3. THERMODYNAMICS & CLIMATE (Termodynamika a prostředí) ---
#define ENABLE_DS18B20          // W001 Thermometer DS18B20 (Teploměr DS18B20)
#define ENABLE_MLX90614         // I005 Non-contact thermometer MLX90614 (Bezkontaktní teploměr MLX90614)
#define ENABLE_MAX6675          // D034 Temperature sensor MAX6675 + K-Type Thermocouple (Čidlo teploty MAX6675 + Termočlánek typu K)
#define ENABLE_BME280           // I002 Barometer, thermometer and hygrometer BME280 (Barometr + teploměr + vlhkoměr BME280)

// --- 4. LIGHT & OPTICS (Světlo a optika) ---
#define ENABLE_LUX              // A018 Light intensity sensor (Čidlo intenzity osvětlení)
#define ENABLE_LUXBH1750        // I032 Light intensity sensor BH1750 in lux (Čidlo intenzity světla BH1750 - hodnota v luxech)
#define ENABLE_VEML6070         // I031 UV radiation sensor with VEML6070 (Čidlo UV záření - s VEML6070)
#define ENABLE_TCS34725         // I011 Color sensor TCS34725 (Čidlo barev TCS34725)
  // --- TCS34725 Configuration (Nastavení pro čidlo barev) ---
  #define TCS34725_LED_DEFAULT_ON 0  
  /* 
   * --- Default LED State Macro (Výchozí stav LED) ---
   * Set to 1 to boot up with LED ON. (Nastav na 1 pro spuštění se zapnutou LED.)
   * Set to 0 to boot up with LED OFF. (Nastav na 0 pro spuštění s vypnutou LED.)
   * Can be toggled at runtime using a double-click on the hardware button. 
   * (Lze přepínat za běhu dvojklikem na HW tlačítko.)
   */
  // Uncomment exactly ONE "PROFILE_" below to optimize Arduino NANO memory.
  // (Odkomentuj přesně JEDEN "PROFILE_" níže pro optimalizaci paměti Arduino NANO.)
  #define PROFILE_BASIC 1       // Lightweight code, HW Button - LED Toggle and White calibration (Základní kód, HW tlačítko - přepínání LED a kalibrace bílé)
      /*
       * MG U01 HW Button - LED Toggle (Double Click) and White calibration (Single Click)
       */ 
  //#define PROFILE_MONITOR 1   // Advanced LUTs, sRGB Gamma, 2-step calibration for emissive screens (Pokročilé LUT, sRGB Gamma, 2-kroková kalibrace pro svítící obrazovky)
      /*
       * --- MONITOR PROFILE CALIBRATION & CONTROL GUIDE ---
       * 1. LED Toggle & Undo (Double Click MG U01 HW Button):
       *    - Double-click the button (two clicks within 600ms) to toggle the built-in LED state (ON/OFF) 
       *      and simultaneously trigger an UNDO action, reverting calibration coefficients to the previous state.
       *      
       * 2. 2-Step Calibration (Single Click on MG U01 HW Button):
       *    - Attention! This advanced color sensor mode requires more memory and the Arduino NANO does not have it.
       *      It is necessary to get memory by "commenting" certain sensors in the config.h file. We recommend commenting out sensors that you do not use often.
       *    - Step 1: Place the sensor on a White area on the screen and single-click. This sets the white reference (cmax).
       *    - Step 2: Place the sensor on a Black area and single-click within 10 seconds. This sets the black point (cmin) to compensate for backlight bleed / IPS glow.
       *    - Note: If more than 10 seconds pass before the second click, the state resets back to Step 1 (White).
       *   
       */  
  //#define PROFILE_REFLECT 1   // Advanced measurement in reflected light - TO BE IMPLEMENTED (Pokročilé měření v odraženém světle - ZATÍM NEIMPLEMENTOVÁNO)

// --- 5. CHEMISTRY, LIQUIDS & GASES (Chemie, kapaliny a plyny) ---
#define ENABLE_PH               // A014 pH sensor (Čidlo pH)
#define ENABLE_CON              // C015 Conductivity and salinity sensor (Čidlo vodivosti a salinity)
#define ENABLE_ORP              // A037 ORP electrode and module (ORP elektroda a modul v 0.1)
#define ENABLE_TURB             // A038 Turbidity sensor TS-300B (Turbidimetr - čidlo zákalu TS-300B)
#define ENABLE_CO2              // C019 Carbon dioxide sensor MH-Z16, PWM out (Čidlo oxidu uhličitého MH-Z16, PWM out)
#define ENABLE_O2               // A020 Oxygen sensor ME2-O2 (Čidlo kyslíku ME2-O2)
#define ENABLE_MQ3              // A025 Alcohol gas sensor MQ-3 (Čidlo alkoholu plyn - MQ-3)
#define ENABLE_MQ2              // A036 LPG, Propane, Methane, Hydrogen gas sensor MQ-2 (Čidlo LPG, Propane, Methane, Hydrogen plyn - MQ-2)

// --- 6. BIOLOGY & MEDICAL (Biologie a zdraví) ---
#define ENABLE_PULS             // A022 Heart rate sensor (Čidlo srdečního tepu)
#define ENABLE_AD8232           // A010 ECG AD8232 (EKG AD8232)

// --- 7. ELECTRICITY (Elektřina) ---
#define ENABLE_DCV25            // A028 DC voltage sensor 0-25 V - voltage divider (Čidlo DC napětí 0-25 V - klasický dělič)
#define ENABLE_DCA5             // A029 DC current sensor 0-5 A with ACS712 (Čidlo DC proudu 0-5 A - s ACS712)
#define ENABLE_DCA30            // A030 DC current sensor 0-30 A with ACS712 (Čidlo DC proudu 0-30 A - s ACS712)

// --- 8. TEST / FUTURE SENSORS ---
//#define ENABLE_HX711            // I107 24-bit AD converter for load cells (24bit AD převodník pro tenzometry)
//#define ENABLE_LED              // L200 LED module for sampling frequency testing (LED modul pro testování vzorkovací frekvence)
//#define ENABLE_B_BOARD          // M040 Output/development module - breadboard (Výstupní/vývojový modul - nepájivé pole)
//#define ENABLE_SPIRO            // A041 Spirometer with pressure sensor MPXV7002DP (Spirometr - tlakoměr MPXV7002DP)
//#define ENABLE_GEIGER           // D042 Geiger counter RadiationD v1.1 module (Geiger čítač - RadiationD v1.1 modul)

// --- 9. DEPRECATED ---
//#define ENABLE_DHT11            // W024 Humidity and temperature sensor DHT11 (Vlhkost a teplota DHT11)

#endif
