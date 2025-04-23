#include "tictactoe_game.h"

#define CELL_SIZE 15
#define BOARD_X 5
#define BOARD_Y 5
#define INFO_X 55
#define BUZZER 13

TicTacToeGame::TicTacToeGame(Adafruit_SH1106G* disp) : display(disp) {
    pinMode(BUZZER, OUTPUT);
    init();
}

void TicTacToeGame::init() {
    // Initialize board
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
    currentPlayer = 1;
    selectorX = 0;
    selectorY = 0;
    gameOver = false;
    vsComputer = true;
}

void TicTacToeGame::moveSelector() {
    if (!gameOver) {
        tone(BUZZER, 1000, 50); // Movement sound
        selectorX = (selectorX + 1) % 3;
        if (selectorX == 0) {
            selectorY = (selectorY + 1) % 3;
        }
    }
}

void TicTacToeGame::makeMove() {
    if (!gameOver && currentPlayer == 1 && board[selectorY][selectorX] == ' ') {
        tone(BUZZER, 1500, 100); // Player move sound
        board[selectorY][selectorX] = 'X';
        if (checkWin()) {
            gameOver = true;
            tone(BUZZER, 2000, 200); // Win sound
            delay(200);
            tone(BUZZER, 2500, 200);
        } else if (checkDraw()) {
            gameOver = true;
            tone(BUZZER, 500, 300); // Draw sound
        } else {
            currentPlayer = 2;
        }
    }
}

void TicTacToeGame::computerMove() {
    // Simple AI: Look for winning move
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = 'O';
                if(checkWin()) return;
                board[i][j] = ' ';
            }
        }
    }
    
    // Block player's winning move
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = 'X';
                if(checkWin()) {
                    board[i][j] = 'O';
                    return;
                }
                board[i][j] = ' ';
            }
        }
    }
    
    // Take center if available
    if(board[1][1] == ' ') {
        board[1][1] = 'O';
        return;
    }
    
    // Take a random empty spot
    int tries = 0;
    while(tries < 50) {
        int i = random(3);
        int j = random(3);
        if(board[i][j] == ' ') {
            board[i][j] = 'O';
            return;
        }
        tries++;
    }
    
    // If random failed, take first empty spot
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') {
                board[i][j] = 'O';
                return;
            }
        }
    }
}

int TicTacToeGame::minimax(bool isMaximizing) {
    if (checkWin()) {
        return isMaximizing ? -1 : 1;
    }
    if (checkDraw()) {
        return 0;
    }
    
    if (isMaximizing) {
        int bestScore = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'O';
                    int score = minimax(false);
                    board[i][j] = ' ';
                    bestScore = max(score, bestScore);
                }
            }
        }
        return bestScore;
    } else {
        int bestScore = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == ' ') {
                    board[i][j] = 'X';
                    int score = minimax(true);
                    board[i][j] = ' ';
                    bestScore = min(score, bestScore);
                }
            }
        }
        return bestScore;
    }
}

bool TicTacToeGame::checkWin() {
    // Check rows, columns and diagonals
    for(int i = 0; i < 3; i++) {
        if(board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return true;
        if(board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return true;
    }
    if(board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return true;
    if(board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return true;
    return false;
}

bool TicTacToeGame::checkDraw() {
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == ' ') return false;
        }
    }
    return !checkWin();
}

void TicTacToeGame::update() {
    if (gameOver) {
        // Check for reset button (IO25)
        if (digitalRead(25) == LOW) {
            tone(BUZZER, 1000, 100); // Reset sound
            init();
            delay(200); // Debounce
        }
        return;
    }

    // Computer's turn
    if (vsComputer && currentPlayer == 2 && !gameOver) {
        computerMove();
        tone(BUZZER, 800, 100); // Computer move sound
        if (checkWin()) {
            gameOver = true;
            tone(BUZZER, 300, 200); // Loss sound
            delay(200);
            tone(BUZZER, 200, 300);
        } else if (checkDraw()) {
            gameOver = true;
            tone(BUZZER, 500, 300); // Draw sound
        } else {
            currentPlayer = 1;
        }
    }
}

void TicTacToeGame::draw() {
    // Draw board lines
    for(int i = 0; i <= 3; i++) {
        display->drawLine(BOARD_X + i * CELL_SIZE, BOARD_Y, 
                         BOARD_X + i * CELL_SIZE, BOARD_Y + 3 * CELL_SIZE, SH110X_WHITE);
        display->drawLine(BOARD_X, BOARD_Y + i * CELL_SIZE, 
                         BOARD_X + 3 * CELL_SIZE, BOARD_Y + i * CELL_SIZE, SH110X_WHITE);
    }

    // Draw X's and O's
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 3; j++) {
            if(board[i][j] == 'X') {
                display->drawLine(BOARD_X + j * CELL_SIZE + 3, BOARD_Y + i * CELL_SIZE + 3,
                                BOARD_X + (j + 1) * CELL_SIZE - 3, BOARD_Y + (i + 1) * CELL_SIZE - 3, SH110X_WHITE);
                display->drawLine(BOARD_X + (j + 1) * CELL_SIZE - 3, BOARD_Y + i * CELL_SIZE + 3,
                                BOARD_X + j * CELL_SIZE + 3, BOARD_Y + (i + 1) * CELL_SIZE - 3, SH110X_WHITE);
            } else if(board[i][j] == 'O') {
                display->drawCircle(BOARD_X + j * CELL_SIZE + CELL_SIZE/2,
                                  BOARD_Y + i * CELL_SIZE + CELL_SIZE/2, 5, SH110X_WHITE);
            }
        }
    }

    // Draw selector if game is not over
    if (!gameOver) {
        display->drawRect(BOARD_X + selectorX * CELL_SIZE, BOARD_Y + selectorY * CELL_SIZE,
                         CELL_SIZE, CELL_SIZE, SH110X_WHITE);
    }

    // Draw game info
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    
    display->setCursor(INFO_X, 10);
    display->print("Tic Tac Toe");
    
    display->setCursor(INFO_X, 25);
    display->print("Player: X");
    display->setCursor(INFO_X, 35);
    display->print("CPU: O");

    // Show game state
    display->setCursor(INFO_X, 50);
    if (gameOver) {
        if (checkDraw()) {
            display->print("Draw!");
        } else {
            if (currentPlayer == 1) {
                display->print("Player wins!");
            } else {
                display->print("CPU wins!");
            }
        }
        // Show reset instruction
        display->setCursor(INFO_X, 55);
    } else {
        if (currentPlayer == 1) {
            display->print("Your turn");
        } else {
            display->print("CPU turn");
        }
    }
}