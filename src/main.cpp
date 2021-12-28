// Kod Zrodlowy - Projektu SMiW

// Autor: Daniel Swietlik
// Wydzial: Politechnika Slaska AEiI
// Kierunek: Informatyka Katowice
// Temat: Projekt urzadzenia kontrolujacego wentylator za pomoca podczerwieni

// Importowanie Bibliotek Arduino

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

volatile unsigned int curUiPage = 0; // 0 => pokazuje aktualna temperature
unsigned int curFanMode = 0; // curFanMode = 0  => Wentylator jest wylaczony

float curTemp = 0.0f; // Aktualna temperatura
volatile float mode1Temp = 0.0f; // curFanMode = 1
volatile float mode2Temp = 0.0f; // curFanMode = 2
volatile float mode3Temp = 0.0f; // curFanMode = 3
bool fanON = false;


IRsend irSend(IR_TRANS_PIN);
LiquidCrystal_I2C display(LCD_I2C, LCD_COLS, LCD_ROWS);
DFRobot_SHT3x temperatureSensor;

// Funkcje wykonujace dzialanie urzadzenia

float roundFPrec(float, int);
// bool isButtonClicked(unsigned int pin);
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
  fanControl();
  displayCurUIPage();
  delay(200);
}

// Button Handlery - Implementacja

// bool isButtonClicked(unsigned int pin) {
//   if (digitalRead(pin) == LOW) {
//     delay(20);
//     if (digitalRead(pin) == LOW)
//       return true;
//   }
//   return false;
// }

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

void displayCurUIPage() {
  curTemp = temperatureSensor.getTemperatureC();
  switch (curUiPage) {
    case 0:
      display.clear();
      display.print("Current Temp");
      display.setCursor(0,1);
      display.print(String(curTemp) + " *C");
      break;
    case 1:
      display.clear();
      display.print("Set - Mode I");
      display.setCursor(0,1);
      display.print(String(mode1Temp) + " *C");
      break;
    case 2:
      display.clear();
      display.print("Set - Mode II");
      display.setCursor(0,1);
      display.print(String(mode2Temp) + " *C");
      break;
    case 3:
      display.clear();
      display.print("Set - Mode III");
      display.setCursor(0,1);
      display.print(String(mode3Temp) + " *C");
      break;
    default:
      break;
  }
}

void fanControl() {
  float ctemp = roundFPrec(curTemp, 1);

  if (ctemp >= mode1Temp && ctemp < mode2Temp && !fanON) { 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      fanON = true;
  }
  else if (ctemp >= mode2Temp && ctemp < mode3Temp) {
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
  }
  else if (ctemp >= mode3Temp) { 
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
  }
  else if (fanON){ 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      fanON = false;
  }
}

float roundFPrec(float value, int prec) {
  int multipl = pow(10, prec);
  return (float)((int)(value * (float)multipl + 0.5)) / (float)multipl; 
}

