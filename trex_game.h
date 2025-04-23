#ifndef TREX_GAME_H
#define TREX_GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "sprites.h"

// Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Game settings
#define DINO_WIDTH 25
#define DINO_HEIGHT 26
#define GROUND_Y 54
#define GRAVITY 1.2
#define JUMP_FORCE -10.0
#define INITIAL_SPEED 4.0
#define MAX_SPEED 7.0
#define ACCELERATION 0.002
#define TREE1_WIDTH 11
#define TREE1_HEIGHT 23
#define TREE2_WIDTH 22
#define TREE2_HEIGHT 23

class TRexGame {
private:
    Adafruit_SH1106G* display;
    float dinoY;
    float dinoVelocity;
    int score;
    bool isGameOver;
    int obstacleX;
    bool isJumping;
    unsigned long gameStartTime;
    bool useTree2;
    float gameSpeed;
    const uint8_t BUZZER = 23; // IO23 for buzzer
    
    bool checkCollision();

public:
    TRexGame(Adafruit_SH1106G* disp);
    void init();
    void update();
    void draw();
    void onJump();
};

#endif