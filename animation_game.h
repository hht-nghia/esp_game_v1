#ifndef ANIMATION_GAME_H
#define ANIMATION_GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class AnimationGame {
private:
    static const int FRAME_DELAY = 42;
    static const int ANIMATION_SWITCH_DELAY = 5000;
    
    static const int GHOST_WIDTH = 48;
    static const int GHOST_HEIGHT = 48;
    static const int COOL_WIDTH = 64;
    static const int COOL_HEIGHT = 64;
    static const int ROCKET_WIDTH = 64;
    static const int ROCKET_HEIGHT = 64;
    
    static const int GHOST_FRAME_COUNT;
    static const int COOL_FRAME_COUNT;
    static const int ROCKET_FRAME_COUNT;
    
    Adafruit_SH1106G* display;
    int currentAnimation;
    int frame;
    unsigned long lastSwitchTime;

public:
    AnimationGame(Adafruit_SH1106G* disp);
    void init();
    void update();
    void draw();
};

#endif