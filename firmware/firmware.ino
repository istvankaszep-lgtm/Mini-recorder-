// ===================================================
// RP2040 Mini Audio Recorder - Végleges Firmware
// Pontos pin-kiosztás alapján
// ===================================================

#include <SPI.h>
#include <SD.h>
#include <U8g2lib.h>

// --- PIN KIOSZTÁS (A te megadásod alapján) ---
const int PIN_SPEAKER      = 14;  // Hangszóró / Buzzer
const int PIN_MIC          = 27;  // Mikrofon (ADC1)
const int PIN_POT          = 26;  // Potenciométer (ADC0)

// Gombok (Pull-up beállítással, lenyomva LOW)
const int PIN_BTN_PLAY     = 11;  
const int PIN_BTN_EXIT     = 12;  
const int PIN_BTN_RECORD   = 10;  

// SD Kártya (SPI busz: MOSI=GPIO19/Clk, MISO/Dat0-1, stb.)
const int PIN_SD_CS        = 16;  // DAT3 / CS
// Megjegyzés a többi SD pinre: CMD->GPIO18(SCK), Clk->GPIO19(TX/MOSI), Dat->GPIO17

// OLED Kijelző (SPI módban)
const int PIN_OLED_CS      = 5;
const int PIN_OLED_RES     = 6;
const int PIN_OLED_DC      = 7;
const int PIN_OLED_D0      = 2;   // SCL / Clock
const int PIN_OLED_D1      = 3;   // SDA / MOSI

// OLED Objektum inicializálása (példa SSD1306 / SH1106 vezérlőhöz SPI-ben)
// Ha 4-vezetékes SPI-t használsz: U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI
U8G2_SSD1306_128X64_NONAME_F_4W_SW_SPI u8g2(U8G2_R0, /* clock=*/ PIN_OLED_D1, /* data=*/ PIN_OLED_D0, /* cs=*/ PIN_OLED_CS, /* dc=*/ PIN_OLED_DC, /* reset=*/ PIN_OLED_RES);

void setup() {
  // Soros monitor indítása
  Serial.begin(115200);
  delay(500);
  Serial.println("RP2040 Audio Recorder indul...");

  // Pin mode-ok beállítása
  pinMode(PIN_SPEAKER, OUTPUT);
  
  pinMode(PIN_BTN_PLAY, INPUT_PULLUP);
  pinMode(PIN_BTN_EXIT, INPUT_PULLUP);
  pinMode(PIN_BTN_RECORD, INPUT_PULLUP);

  // OLED kijelző indítása
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_nc_tr08_tf);
  u8g2.drawStr(0, 10, "RP2040 Audio Rec");
  u8g2.drawStr(0, 25, "Rendszer inditva...");
  u8g2.sendBuffer();

  // SD Kártya inicializálás
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("SD kártya hiba vagy nincs benne!");
    u8g2.drawStr(0, 40, "SD Karta Hiba!");
    u8g2.sendBuffer();
  } else {
    Serial.println("SD kártya sikeresen csatlakoztatva.");
    u8g2.drawStr(0, 40, "SD Karta OK");
    u8g2.sendBuffer();
  }

  // Indulási hangjelzés a hangszórón
  tone(PIN_SPEAKER, 1200, 150);
  delay(200);
  tone(PIN_SPEAKER, 1800, 150);
}

void loop() {
  // 1. Érzékelők beolvasása
  int micValue = analogRead(PIN_MIC);       // Mikrofon jelek (0 - 4095)
  int potValue = analogRead(PIN_POT);       // Hangerő / beállítás potméter
  
  // Gombok állapota (LOW = lenyomva a PULLUP miatt)
  bool isRecPressed   = (digitalRead(PIN_BTN_RECORD) == LOW);
  bool isPlayPressed  = (digitalRead(PIN_BTN_PLAY) == LOW);
  bool isExitPressed  = (digitalRead(PIN_BTN_EXIT) == LOW);

  // 2. Gomblogika kezelése
  if (isRecPressed) {
    Serial.println("FELVÉTEL indítása...");
    tone(PIN_SPEAKER, 900, 100);
    
    updateDisplay("Allapot: Felvetel...");
    // Ide jöhet majd a SD-re író ciklus
    delay(300); // pergésmentesítés
  }

  if (isPlayPressed) {
    Serial.println("LEJÁTSZÁS indítása...");
    tone(PIN_SPEAKER, 1500, 100);
    
    updateDisplay("Allapot: Lejatszas...");
    delay(300);
  }

  if (isExitPressed) {
    Serial.println("EXIT / STOP gomb megnyomva.");
    tone(PIN_SPEAKER, 600, 100);
    
    updateDisplay("Allapot: Megallitva");
    delay(300);
  }

  // 3. Képernyő frissítése a potméter / mikrofon adataival (opcionális teszt)
  // Használhatod a potValue-t a hangerőhöz vagy a kijelző frissítéséhez
}

// Segédfüggvény az OLED szöírásra
void updateDisplay(String statusText) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_nc_tr08_tf);
  u8g2.drawStr(0, 15, "Mini Audio Rec");
  u8g2.setCursor(0, 35);
  u8g2.print(statusText);
  u8g2.sendBuffer();
}