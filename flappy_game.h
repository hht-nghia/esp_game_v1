#ifndef FLAPPY_GAME_H
#define FLAPPY_GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "sprites.h"

class FlappyGame {
private:
    Adafruit_SH1106G* display;
    float birdY;
    float birdVelocity;
    int score;
    bool isGameOver;
    int wall_x[2];
    int wall_y[2];
    int wall_gap;
    int wall_width;
    const uint8_t BUZZER = 23; // IO23 for buzzer
    
    static const int SPRITE_HEIGHT = 16;
    static const int SPRITE_WIDTH = 16;
    static constexpr float GRAVITY = 1.0f;      // Tăng gravity để chim rơi nhanh hơn
    static constexpr float JUMP_FORCE = -4.0f;  // Giống như momentum = -4 trong code mẫu
    static const int WALL_SPEED = 4;            // Giống như wall_x[i] -= 4
    static const int GAME_SPEED = 50;           // Delay giữa các frame

public:
    FlappyGame(Adafruit_SH1106G* disp);
    void init();
    void update();
    void draw();
    void onJump();
    bool isOver() { return isGameOver; }
};

#endif