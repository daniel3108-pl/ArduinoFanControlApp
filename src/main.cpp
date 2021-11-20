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

// Kody podczerwieni pilota do wentylatora | rc = remote control
#define rcOn    0x00
#define rcOff   0x00
#define rcMode1 0x00
#define rcMode2 0x00
#define rcMode3 0x00

// Definiowanie Pinow modulow i przyciskow itp
#define irTransPIN  1
#define irRecvPIN   1
#define UP          1
#define DOWN        1
#define SET         1

// Definiowanie obiektow modulow i przyciskow, zmiennych
float curTemp   = 0;
float mode1Temp = 0;
float mode2Temp = 0;
float mode3Temp = 0;

IRrecv irrecv(irRecvPIN);
decode_results results;


// Funkcje wykonujace dzialanie urzadzenia

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}

void irRAWSetup(){

}