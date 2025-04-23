#pragma once

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class StarWarGame {
private:
    Adafruit_SH1106G* display;
    // Game bitmap data
    static const unsigned char PROGMEM dioda16[];

    // Game variables
    int metx = 0, mety = 0, postoji = 0;
    int nep = 8, smjer = 0, go = 0;
    int rx = 95, ry = 0;
    int rx2 = 95, ry2 = 0;
    int rx3 = 95, ry3 = 0;
    int rx4 = 95, ry4 = 0;
    int bodovi = 0;
    int brzina = 3;
    int bkugle = 1;
    int najmanja = 600;
    int najveca = 1200;
    int promjer = 10;
    int zivoti = 5;
    int poc = 0;
    int ispaljeno = 0;
    int nivo = 1;
    int centar = 95;
    int poz = 30;
    unsigned long pocetno = 0;
    unsigned long odabrano = 0;
    unsigned long trenutno = 0;
    unsigned long nivovrije = 0;
    bool isRunning = false;

    // Button pins
    const uint8_t BTN_UP = 26;    // io26
    const uint8_t BTN_DOWN = 25;  // io25  
    const uint8_t BTN_SHOOT = 32; // io32
    const uint8_t BTN_MENU = 33;  // io33
    const uint8_t BUZZER = 23;    // io23

    void beep(int note, int duration);
    void ponovo();

public:
    StarWarGame(Adafruit_SH1106G* disp);
    void init();
    void update();
    void draw();
    bool isGameRunning() { return isRunning; }
    void setRunning(bool running) { isRunning = running; }
};