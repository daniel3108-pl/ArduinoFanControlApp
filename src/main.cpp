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
#include <math.h>

// Definiowanie kodow podczerwieni pilota do wentylatora | rc = remote control
#define rcOnOffLen 1
unsigned int rcOnOffRawData[rcOnOffLen] = {
  0
};

#define rcChngModeLen 1
unsigned int rcChngModeRawData[rcChngModeLen] = {
  0
};

// Definiowanie Pinow modulow i przyciskow itp
#define UP            0x1
#define DOWN          0x2
#define SET           0x3
#define IR_TRANS_PIN  0x4
#define IR_RECV_PIN   0x5

// Adresy I2C modulow lcd 2x16 i czujnika temperatury sht31-f
#define SHT31F_I2C  0x45
#define LCD_I2C     0x27
#define LCD_COLS    16
#define LCD_ROWS    2  

// Definiowanie obiektow modulow i przyciskow, zmiennych

unsigned int curUiPage = 0; // 0 => pokazuje aktualna temperature
unsigned int curFanMode = 0; // curFanMode = 0  => Wentylator jest wylaczony
float curTemp = 0.0f;
float mode1Temp = 0.0f; // curFanMode = 1
float mode2Temp = 0.0f; // curFanMode = 2
float mode3Temp = 0.0f; // curFanMode = 3


IRrecv irrecv(IR_RECV_PIN);
decode_results results;

IRsend irSend(IR_TRANS_PIN);
LiquidCrystal_I2C display(LCD_I2C, LCD_COLS, LCD_ROWS);
DFRobot_SHT3x temperatureSensor;

// Funkcje wykonujace dzialanie urzadzenia
float roundFPrec(float, int);
void irRAWDataScraping();
bool isButtonClicked(unsigned int);
void userInterfaceLogic();
void displayCurUIPage();
void fanControl();

// Implementacja funkcji urzadzenia

void setup() {
  Serial.begin(9600);

  pinMode(UP, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(SET, INPUT);

  display.begin(LCD_COLS, LCD_ROWS, LCD_5x8DOTS);
  temperatureSensor.begin();
  irSend.begin(IR_TRANS_PIN, false, 0);
  irrecv.begin(IR_RECV_PIN);
}
  
void loop() {
  curTemp = temperatureSensor.getTemperatureC();

}

void irRAWDataScraping(){

}

bool isButtonClicked(unsigned int pin){
  if (digitalRead(pin) == HIGH) {
    delay(20);
    if (digitalRead(pin) == HIGH)
      return true;
  }
  return false;
}

void userInterfaceLogic(){
  if (isButtonClicked(UP)) { 

  } 
  else if (isButtonClicked(DOWN)) {

  }
  else if (isButtonClicked(SET)) { 

  }
  else {

  }
}

void displayCurUIPage(){
  switch (curUiPage){
    case 0:
      break;
    case 1:
      break;
    case 2:
      break;
    case 3:
      break;
    default:
      break;
  }
}

void fanControl(){

  float ctemp = roundFPrec(curTemp, 1);

  if( ctemp >= mode1Temp && ctemp < mode2Temp ) { 
    
  }
  else if( ctemp >= mode2Temp && ctemp < mode3Temp ) {

  }
  else if( ctemp >= mode3Temp ) { 

  }
  else { 

  }
}

float roundFPrec(float value, int prec){
  int multipl = pow(10, prec);
  return (float)((int)(value * (float)multipl + 0.5)) / (float)multipl; 
}

