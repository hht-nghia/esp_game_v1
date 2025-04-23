#ifndef TICTACTOE_GAME_H
#define TICTACTOE_GAME_H

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

class TicTacToeGame {
private:
    Adafruit_SH1106G* display;
    char board[3][3];
    int currentPlayer; // 1 for Player, 2 for CPU
    int selectorX;
    int selectorY;
    bool gameOver;
    bool vsComputer;
    const uint8_t BUZZER = 23; // IO23 for buzzer
    
    bool checkWin();
    bool checkDraw();
    void computerMove();
    int minimax(bool isMaximizing);
    bool isMovesLeft();

public:
    TicTacToeGame(Adafruit_SH1106G* disp);
    void init();
    void update();
    void draw();
    void moveSelector();
    void makeMove();
};

#endif