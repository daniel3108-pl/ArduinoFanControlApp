
/* 
 Autor: Daniel Swietlik
 Wydzial: Politechnika Slaska AEiI
 Kierunek: Informatyka Katowice

 Projekt SMIW
 Temat: Projekt urzadzenia kontrolujacego wentylator za pomoca podczerwieni

 Kod Zrodlowy oprogramowania dla urzadzenia
*/


// Include Bibliotek Arduino i modulow

#include <Arduino.h>
#include <DFRobot_SHT3x.h>
#include <LiquidCrystal_I2C.h>
#include <IRremote.h>
#include <string.h>

// Definiowanie kodow podczerwieni pilota do wentylatora | rc = remote control

#define RC_ONOFF      0xFF807F
#define RC_NEXTMODE   0xFF807F
#define RC_REPEAT     0xFFFFFF

// Definiowanie Pinow modulow i przyciskow itp

#define UP            0x0
#define DOWN          0x1
#define SET           0x2
#define IR_TRANS_PIN  0x3

// Adresy I2C modulow lcd 2x16 i czujnika temperatury sht31-f

#define SHT31F_I2C  0x45
#define LCD_I2C     0x27
#define LCD_COLS    16
#define LCD_ROWS    2  

// Definiowanie obiektow modulow i przyciskow, zmiennych

unsigned int prevUiPage = 4; // poprzednia aktywna strona interface'u | 4 => Wartosc zabroniona, zeby sie wyswietlil za 1 razem ui
volatile unsigned int curUiPage = 0; // aktualna strona interface'u
unsigned int curFanMode = 0; // curFanMode = 0  => Wentylator jest wylaczony
bool fanON = false; // Czy wetylator jest wlaczony czy nie 

float curTemp = 0.0f; // Aktualna temperatura
volatile float mode1Temp = 0.0f; // curFanMode = 1
volatile float mode2Temp = 0.0f; // curFanMode = 2
volatile float mode3Temp = 0.0f; // curFanMode = 3

// Obiekty modulow

IRsend irSend(IR_TRANS_PIN);
LiquidCrystal_I2C display(LCD_I2C, LCD_COLS, LCD_ROWS);
DFRobot_SHT3x temperatureSensor;

// Naglowki funkcji wykonujace dzialanie urzadzenia

float roundFPrec(float, int);
void setButtonHandler();
void upButtonHandler();
void downButtonHandler();
void displayCurUIPage();
void fanControl();

// Implementacja funkcji urzadzenia

void setup() {

  pinMode(SET, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(UP), upButtonHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN), downButtonHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(SET), setButtonHandler, FALLING);

  display.init();
  display.print("Starting...");
  display.backlight();
  Serial.begin(9600);

}
  
void loop(){
  curTemp = temperatureSensor.getTemperatureC();
  fanControl();
  displayCurUIPage();
  delay(200);
}

// Button Handlery - Implementacja przerwan

void setButtonHandler(){
  curUiPage++;
  curUiPage %= 4;
}

void upButtonHandler(){
  switch (curUiPage)
  {
  case 1:
    mode1Temp += 0.5f;
    break;
  case 2:
    mode2Temp += 0.5f;
    break;
  case 3:
    mode3Temp += 0.5f;
    break;
  default:
    break;
  }
}

void downButtonHandler(){
  switch (curUiPage)
  {
  case 1:
    mode1Temp -= 0.5f;
    break;
  case 2:
    mode2Temp -= 0.5f;
    break;
  case 3:
    mode3Temp -= 0.5f;
    break;
  default:
    break;
  }
}

// Funkcja wyswietlajaca aktualna strone interfejsu uzytkownika na wyswietlaczu
void displayCurUIPage() {

  if (curUiPage == prevUiPage)
    return;
  
  switch (curUiPage) {
    case 0:
      display.clear();
      display.print("Cur Temp | " + String(curFanMode) + "M");
      display.setCursor(0,1);
      display.print(String(curTemp) + " oC");
      prevUiPage = curUiPage;
      break;
    case 1:
      display.clear();
      display.print("Set - Mode I");
      display.setCursor(0,1);
      display.print(String(mode1Temp) + " oC");
      prevUiPage = curUiPage;
      break;
    case 2:
      display.clear();
      display.print("Set - Mode II");
      display.setCursor(0,1);
      display.print(String(mode2Temp) + " oC");
      prevUiPage = curUiPage;
      break;
    case 3:
      display.clear();
      display.print("Set - Mode III");
      display.setCursor(0,1);
      display.print(String(mode3Temp) + " oC");
      prevUiPage = curUiPage;
      break;
    default:
      break;
  }
}

// Funkcja ktora kontroluje wiatrak na podstawie temperatury trybow uzytkownika
void fanControl() {
  float ctemp = roundFPrec(curTemp, 1); // 21 22 24 | 24.5, curFanMode = 3

  if (ctemp >= mode1Temp && ctemp < mode2Temp && curFanMode != 1 ) { 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      fanON = true;
      curFanMode = 1;
      return;
  }
  else if (ctemp >= mode2Temp && ctemp < mode3Temp && curFanMode != 2) {
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      curFanMode = 2;
      return;
  }
  else if (ctemp >= mode3Temp && curFanMode != 3) { 
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      curFanMode = 3;
      return;
  }
  else if (ctemp < mode1Temp && curFanMode != 0){ 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      fanON = false;
      curFanMode = 0;
      return;
  }
}

// Prosta funkcja zaokraglajaca float'y
float roundFPrec(float value, int prec) {
  int multipl = pow(10, prec);
  return (float)((int)(value * (float)multipl + 0.5)) / (float)multipl; 
}

