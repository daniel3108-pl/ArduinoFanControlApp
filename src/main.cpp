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

// Kody pilota wentylatora
enum RemoteCodes { 
  OnOff = 0x01, Mode1 = 0x02,
  Mode2 = 0x03, Mode3 = 0x04
};

// Definiowanie Pinow modulow i przyciskow itp
#define irTransPIN 1
#define irRecvPIN 1
#define UP 1
#define DOWN 1
#define SET 1

// Definiowanie obiektow modulow i przyciskow, zmiennych
float curTemp = 0;
float mode1Temp = 0;
float mode2Temp = 0;
float mode3Temp = 0;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

void irRAWSetup(){

}