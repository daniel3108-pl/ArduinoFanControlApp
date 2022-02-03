
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
void sendNextModeSignal();

// Implementacja funkcji urzadzenia

void setup() {

  pinMode(SET, INPUT_PULLUP);
  pinMode(UP, INPUT_PULLUP);
  pinMode(DOWN, INPUT_PULLUP);
  
  attachInterrupt(digitalPinToInterrupt(UP), upButtonHandler, FALLING);
  attachInterrupt(digitalPinToInterrupt(DOWN), downButtonHandler, FALLING);
  //attachInterrupt(digitalPinToInterrupt(SET), setButtonHandler, FALLING);

  if (EEPROM.read(Mode1TempADR) == 0) {
    EEPROM.write(Mode1TempADR, 22.5f);
    EEPROM.write(Mode2TempADR, 23.0f);
    EEPROM.write(Mode3TempADR, 23.5f);
  }

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
  // Na podstawie aktualnej strony wybiera, ktora wartosc temperatury zwiększyć
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
  changeValue = true; // ustalenie, ze zmienila sie wartosc, by wyrenderowac zmiane na lcd
  delay(100);
}

void downButtonHandler(){
  // Na podstawie aktualnej strony wybiera, ktora wartosc temperatury zmniejszyć
    switch (curUiPage)
    {
    case 1:
      mode1Temp -= 0.5f;
      break;
    case 2:
      mode2Temp -= mode2Temp - 0.5f > mode1Temp ? 0.5f : 0.0f;
      break;
    case 3:
      mode3Temp -= mode3Temp - 0.5f > mode2Temp? 0.5f : 0.0f;
      break;
    default:
      break;
    }
    changeValue = true;
    delay(100);
}

// Funkcja wyswietlajaca aktualna strone interfejsu uzytkownika na wyswietlaczu
void displayCurUIPage() {

  // Sprawdza czy nastąpila zmiana wartości temperatury lub w ustawieniach
  // lub zmieniła się storna, jeśli nie funkcja zwraca void.
  if (curUiPage == prevUiPage and changeValue == false)
    return;
  
  // Switch odpowiedzialny za wypisanie odpowiedniego menu na lcd
  switch (curUiPage) {
    case 0:
      display.clear();
      display.print("Cur Temp | " + (curFanMode != 0 ? (String(curFanMode)  + "M") : "OFF"));
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
  // ustawienie zmiennej, ze nie nastapila zmiana po wyrenderowaniu ekranu
  changeValue = false;
  prevUiPage = curUiPage;
}

// Funkcja ktora kontroluje wiatrak na podstawie temperatury trybow uzytkownika
void fanControl() {
  float ctemp = roundFPrec(curTemp, 1); 

  // gdy poprzednio wentylator byl wylaczony i przekroczono granice temperatury dla 1 trybu
  if (ctemp >= mode1Temp && curFanMode == 0) { 
      // wyslanie odpowiednich sygnalow by uruchomic i przejsc do pierwszego trybu
      irSend.sendNEC(RC_ONOFF, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      irSend.sendNEC(RC_REPEAT, 32);
      delay(50);
      sendNextModeSignal()
      sendNextModeSignal()
      fanON = true;
      curFanMode = 1; // ustawienie aktualnego poziomu mocy wentylatora
      changeValue = true; // zaznaczenie ze zmienila sie wartosc trybu
      // aby wyswietlacz wyrenderowal zmiane
      return;
  }
  // gdy byl uruchomiony 1 tryb poprzednio i przekroczono granice 2 trybu
  else if (ctemp >= mode2Temp && curFanMode == 1 && fanON) {
      // przelączenie do następnego trybu prędkości na wentylatorze
      sendNextModeSignal();
      curFanMode = 2;
      changeValue = true;
      return;
  }
  // gdy byl uruchomiony 2 tryb poprzenio i przekroczono granice 3 trybu
  else if (ctemp >= mode3Temp && curFanMode == 2 && fanON) { 
      sendNextModeSignal();
      curFanMode = 3;
      changeValue = true;
      return;
  }
  // gdy spadła temperatura ponizej trybu 1
  else if (ctemp < mode1Temp && fanON){ 
      // wyłączenie wentylatora
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
  // Gdy spadła z temperatura poniej 3 trybu po poprzednim uruchomieniu go
  else if (curFanMode == 3 && ctemp < mode3Temp && fanON) {
      sendNextModeSignal();
      sendNextModeSignal();
      changeValue = true;
      // zmniejszenie zmiennej zapamiętującej aktualny stan
      // mocy wentylatora
      curFanMode--;
  }
  // Gdy spadła z temperatura poniej 2 trybu po poprzednim uruchomieniu go
  else if (curFanMode == 2 && ctemp < mode2Temp && fanON) {
      sendNextModeSignal();
      sendNextModeSignal();
      changeValue = true;
      curFanMode--;
  }
}

// Funkcja wysylajaca sygnal by wlaczyc nastepny tryb predkosci
// w wentylatorze
void sendNextModeSignal() {
  irSend.sendNEC(RC_NEXTMODE, 32);
  delay(50);
  irSend.sendNEC(RC_REPEAT, 32);
  delay(50);
  irSend.sendNEC(RC_REPEAT, 32);
  delay(50);
}

// Prosta funkcja zaokraglajaca float'y
float roundFPrec(float value, int prec) {
  int multipl = pow(10, prec);
  return (float)((int)(value * (float)multipl + 0.5)) / (float)multipl; 
}
