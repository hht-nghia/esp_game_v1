#ifndef GAME_H
#define GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "flappy_game.h"
#include "animation_game.h"
#include "trex_game.h"
#include "tictactoe_game.h"
#include "starwar_game.h"

// Pin definitions
#define BUTTON_MOVE 25  // IO25
#define BUTTON_ACTION 33 // IO33
#define BATTERY_PIN 34  // IO34 for battery monitoring

// Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

// Battery voltage conversion
#define BAT_MAX_V 4.2    // Pin lithium max voltage 
#define BAT_MIN_V 3.3    // Pin lithium min voltage
#define ADC_SCALE 2.17    // Voltage divider scale

// Game states
enum GameState {
    SPLASH_SCREEN,
    MENU,
    PLAYING_TREX,
    PLAYING_TICTACTOE,
    PLAYING_FLAPPY,
    PLAYING_ANIMATION,
    PLAYING_STARWAR
};

// Forward declarations
class TRexGame;
class TicTacToeGame;
class FlappyGame;
class AnimationGame;
class StarWarGame;

class GameManager {
private:
    Adafruit_SH1106G* display;
    GameState currentState;
    int menuSelection;
    unsigned long buttonPressStartTime;
    unsigned long lastButtonPressTime;
    bool isLongPress;
    
    TRexGame* trexGame;
    TicTacToeGame* tictactoeGame;
    FlappyGame* flappyGame;
    AnimationGame* animationGame;
    StarWarGame* starWarGame;

    void drawSplashScreen();
    void drawMenu();
    void handleMenu();
    void handleButtonPress(bool isMove, bool isPressed);
    bool checkLongPress();
    int getBatteryPercentage();

public:
    GameManager(Adafruit_SH1106G* disp);
    ~GameManager();
    
    void init();
    void update();
    void draw();
    void handleInput();
};

#endif