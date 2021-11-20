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
#define UP          1
#define DOWN        2
#define SET         3
#define irTransPIN  4
#define irRecvPIN   5

// Adresy I2C modulow lcd 2x16 i czujnika temperatury sht31-f
#define LCD_I2C     0x27
#define SHT31F_I2C  0x44

// Definiowanie obiektow modulow i przyciskow, zmiennych
unsigned int curFanMode = 0; // curFanMode = 0  => Wentylator jest wylaczony
float curTemp = 0.0f;

float mode1Temp = 0.0f; // curFanMode = 1
float mode2Temp = 0.0f; // curFanMode = 2
float mode3Temp = 0.0f; // curFanMode = 3


// Do zdobycia kodow pilota ir
IRrecv irrecv(irRecvPIN);
decode_results results;
// IRsend irSend(irTransPIN);

// Funkcje wykonujace dzialanie urzadzenia

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

void irRAWSetup(){

}