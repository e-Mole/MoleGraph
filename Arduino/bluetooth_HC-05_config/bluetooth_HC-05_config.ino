/*
 * Setting up the HC-05 Bluetooth module for use within the MoleGraph Shield (https://www.e-mole.cz/diy/molegraph-shield).
 * Nastavení Bluetooth modulu HC-05 pro použití v rámci MoleGraph Shieldu (https://www.e-mole.cz/diy/molegraph-shield).
 */

//The SoftwareSerial library needs to be installed in the Arduino IDE.
//V Arduino IDE je třeba nainstalovat knihovnu SoftwareSerial.
#include <SoftwareSerial.h>

//Connect the HC-05 module to Arduino pins 10 and 11 (RX, TX)
//Připojte HC-05 modul na piny Arduina 10 a 11 (RX, TX)
SoftwareSerial mySerial(10, 11); // RX, TX

String command = ""; // Stores response of the HC-05 Bluetooth device / Ukládá odezvu zařízení HC-05 Bluetooth


void setup() {
  pinMode(9, OUTPUT);  // this pin will pull the HC-05 pin 34 (key pin OR EN pin) HIGH to switch module to AT mode / tento pin přepne HC-05 pin 34 (KEY pin nebo pin EN) do stavu HIGH, což je přepnutí modulu do režimu AT
  digitalWrite(9, HIGH);
  
  // Open serial communications to Arduino:
  // Otevřete sériovou komunikaci s Arduinem:
  Serial.begin(9600);
  Serial.println("Type AT commands!");
  
  // Open serial communications to HC-05 module:
  // Otevřete sériovou komunikaci s HC-05 modulem:
  mySerial.begin(38400);  //Communication speed in AT mode is 38400bps, for HC-05 - CR end line (! - officialy needs CR LF) / Komunikační rychlost v režimu AT je vždy 38400 bps, pro HC-05 - koncová linka CR (! - oficiálně potřebuje CR LF)

  // Required AT commands / Potřebné AT příkazy:
  // AT+UART=115200,0,0\r\n       //Setting communication parameters, speed 115200 bps / Nastavení parametrů komunikace, rychlost 115200 bps
  // AT+NAME=MGA2-220311\r\n      //Change name to MGA2-220311, last 22-03-01 YY-MM-ID / Změní název na MGA2-220311, poslední 22-03-01 YY-MM-ID
  // AT+PSWD="0000"               //Set passcode for pairing (may appear as Passkey, PIN code, PIN number, ...) / nastavte přístupový kód pro párování (může se zbrazovat jako Passkey, PIN kód, PIN číslo, ...)
  
  /*  AT commands are entered from the Serial Monitor command line in the Arduino IDE 
   *  (can be copied from the code, without the leading //!). 
   *  Typically We will set the device name and access code at our discretion. (NAME, PSWD)
   *  AT příkazy se zadávají z příkazového řádku Serial Monitor v Arduino IDE 
   *  (lze zkopírovat z kódu, bez úvodního //!). 
   *  Obvykle nastavíme název zařízení a přístupový kód podle svého uvážení. (NAME, PSWD)
   */
  
  delay(10); 
}

void loop() {
  // Read device output if available.
  // Čtení výstupu zařízení, pokud je k dispozici.
  if (mySerial.available()) {
    while(mySerial.available()) { // While there is more to be read, keep reading / Dokud je co číst, čteme dál
      command += (char)mySerial.read();
    }
    
    Serial.println(command);
    command = ""; // No repeats / Žádné opakování
  }
  
  // Read user input if available.
  // Čtení vstupu uživatele, pokud je k dispozici
  if (Serial.available()){
    delay(10); // The delay is necessary to get this working! / Aby to fungovalo, je nutné zpoždění!
    mySerial.write(Serial.read());
  }
}
