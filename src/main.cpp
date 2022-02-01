
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
#include <EEPROM.h>

// Definiowanie kodow podczerwieni pilota do wentylatora | rc = remote control

#define RC_ONOFF      0xFF807F
#define RC_NEXTMODE   0xFF20DF
#define RC_REPEAT     0xFFFFFF

// Definiowanie Pinow modulow i przyciskow itp

#define UP            0x7
#define DOWN          0x0
#define SET           0x6
#define IR_TRANS_PIN  0x5

// Adresy I2C modulow lcd 2x16 i czujnika temperatury sht31-f

#define SHT31F_I2C  0x45
#define LCD_I2C     0x27
#define LCD_COLS    16
#define LCD_ROWS    2  

typedef unsigned int u_int;

// Definiowanie obiektow modulow i przyciskow, zmiennych

unsigned int prevUiPage = 4; // poprzednia aktywna strona interface'u | 4 => Wartosc zabroniona, zeby sie wyswietlil za 1 razem ui
volatile unsigned int curUiPage = 0; // aktualna strona interface'u
unsigned int curFanMode = 0; // curFanMode = 0  => Wentylator jest wylaczony
bool fanON = false; // Czy wetylator jest wlaczony czy nie 

float curTemp = 0.0f; // Aktualna temperatura
float prevTemp = curTemp;

int Mode1TempADR = 1;
volatile float mode1Temp = 0.0f; // curFanMode = 1

int Mode2TempADR = 2;
volatile float mode2Temp = 0.0f; // curFanMode = 2

int Mode3TempADR = 3;
volatile float mode3Temp = 0.0f; // curFanMode = 3

volatile bool changeValue = false;
bool previousDownState = true;
bool previousSetState = true;

// Obiekty modulow

IRsend irSend;
LiquidCrystal_I2C display(LCD_I2C, LCD_COLS, LCD_ROWS);
DFRobot_SHT3x temperatureSensor;

// Naglowki funkcji wykonujace dzialanie urzadzenia

float roundFPrec(float, int);
void setButtonHandler();
void upButtonHandler();
void downButtonHandler();
void displayCurUIPage();
void fanControl();
bool checkButtonFalling(u_int pin);
void sendFanSignal(u_int signal);

// Implementacja funkcji urzadzenia

void setup() {

  pinMode(SET, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(UP), upButtonHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN), downButtonHandler, FALLING);
  //attachInterrupt(digitalPinToInterrupt(SET), setButtonHandler, FALLING);

  // if (EEPROM.read(1023) != 'T') {
  //   EEPROM.write(Mode1TempADR, 24.0f);
  //   EEPROM.write(Mode2TempADR, 23.0f);
  //   EEPROM.write(Mode3TempADR, 23.5f);
  // }

  mode1Temp = EEPROM.read(Mode1TempADR);
  mode2Temp = EEPROM.read(Mode2TempADR);
  mode3Temp = EEPROM.read(Mode3TempADR);

  display.init();
  display.print("Starting...");
  display.backlight();
  temperatureSensor.begin();
  irSend.begin(IR_TRANS_PIN, true, 0U);
  Serial.begin(9600);

}
  
void loop(){
  if (curUiPage == 0) {
      curTemp = temperatureSensor.getTemperatureC();
      if (roundFPrec(prevTemp, 1) != roundFPrec(curTemp, 1))
        changeValue = true;
      else
        changeValue = false;
      prevTemp = curTemp;
      fanControl();
  }
  
  setButtonHandler();
  displayCurUIPage();
  Serial.println(mode1Temp);
  Serial.println(digitalRead(DOWN));
  delay(300);
}

// Button Handlery - Implementacja przerwan
bool checkButtonFalling(u_int pin, bool previousState){
  if (previousState == HIGH and previousState != digitalRead(pin)) {
    if (digitalRead(pin) == LOW) {
      previousState = false;
      return true;
    }
  }
  previousState = true;
  return false;
}
void setButtonHandler(){
  if (checkButtonFalling(SET, previousSetState)) {
    
      switch(curUiPage){
        case 1:
          EEPROM.write(Mode1TempADR, mode1Temp);
          if (mode2Temp <= mode1Temp)
            mode2Temp = mode1Temp + 0.5f;
          break;
        case 2:    
          EEPROM.write(Mode2TempADR, mode2Temp);
          if (mode3Temp <= mode2Temp)
              mode3Temp = mode2Temp + 0.5f;
          break;
        case 3:
          EEPROM.write(Mode3TempADR, mode3Temp);
          break;
      }
      curUiPage++;
      curUiPage %= 4;
      
      changeValue = false;
  }
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
  changeValue = true;
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
    changeValue = true;
}

// Funkcja wyswietlajaca aktualna strone interfejsu uzytkownika na wyswietlaczu
void displayCurUIPage() {

  if (curUiPage == prevUiPage and changeValue == false)
    return;
  
  switch (curUiPage) {
    case 0:
      display.clear();
      display.print("Cur Temp | " + String(curFanMode) + "M");
      display.setCursor(0,1);
      display.print(String(roundFPrec(curTemp, 1)) + " oC");
      break;
    case 1:
      display.clear();
      display.print("Set - Mode I");
      display.setCursor(0,1);
      display.print(String(mode1Temp) + " oC");
      break;
    case 2:
      display.clear();
      display.print("Set - Mode II");
      display.setCursor(0,1);
      display.print(String(mode2Temp) + " oC");
      break;
    case 3:
      display.clear();
      display.print("Set - Mode III");
      display.setCursor(0,1);
      display.print(String(mode3Temp) + " oC");
      break;
    default:
      break;
  }
  changeValue = false;
  prevUiPage = curUiPage;
}

// Funkcja ktora kontroluje wiatrak na podstawie temperatury trybow uzytkownika
void fanControl() {
  float ctemp = roundFPrec(curTemp, 1); // 21 22 24 | 24.5, curFanMode = 3

  if (ctemp >= mode1Temp && curFanMode == 0) { 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(20);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      fanON = true;
      curFanMode = 1;
      changeValue = true;
      return;
  }
  else if (ctemp >= mode2Temp && curFanMode == 1) {
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);

      Serial.println("wyslano");
      curFanMode = 2;
      changeValue = true;
      return;
  }
  else if (ctemp >= mode3Temp && curFanMode == 2) { 
      irSend.sendNEC(RC_NEXTMODE, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      curFanMode = 3;
      changeValue = true;
      return;
  }
  else if (ctemp < mode1Temp && curFanMode == 0 && fanON){ 
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      fanON = false;
      curFanMode = 0;
      changeValue = true;
      return;
  }
  // else if (curFanMode == 3 && ctemp < mode3Temp)
  //   curFanMode--;
  // else if (curFanMode == 2 && ctemp < mode2Temp)
  //   curFanMode--;
  // else if (curFanMode == 1 && ctemp < mode1Temp)
  //   curFanMode--;


  delay(200);
}

// Prosta funkcja zaokraglajaca float'y
float roundFPrec(float value, int prec) {
  int multipl = pow(10, prec);
  return (float)((int)(value * (float)multipl + 0.5)) / (float)multipl; 
}

void sendFanSignal(u_int signal) {
  irSend.sendNEC(signal, 32);
  delay(50);
  irSend.sendNEC(RC_REPEAT, 32);
  delay(50);
  irSend.sendNEC(RC_REPEAT, 32);
  delay(50);
}