#include "trex_game.h"

TRexGame::TRexGame(Adafruit_SH1106G* disp) : display(disp) {
    pinMode(BUZZER, OUTPUT);
    init();
}

void TRexGame::init() {
    dinoY = GROUND_Y - DINO_HEIGHT;
    dinoVelocity = 0;
    score = 0;
    isGameOver = false;
    obstacleX = SCREEN_WIDTH;
    isJumping = false;
    gameStartTime = millis();
    gameSpeed = INITIAL_SPEED;
    useTree2 = false;

    // Play start sound
    tone(BUZZER, 1000, 100);
    delay(100);
    tone(BUZZER, 1500, 100);
}

void TRexGame::update() {
    if (isGameOver) {
        // Check for reset button (IO25)
        if (digitalRead(25) == LOW) {
            init();
            delay(200); // Debounce
            return;
        }
        return;
    }

    // Increase speed over time
    if (gameSpeed < MAX_SPEED) {
        gameSpeed += ACCELERATION;
    }

    // Update dino position
    dinoVelocity += GRAVITY;
    dinoY += dinoVelocity;

    // Keep dino above ground
    if (dinoY > GROUND_Y - DINO_HEIGHT) {
        dinoY = GROUND_Y - DINO_HEIGHT;
        dinoVelocity = 0;
        isJumping = false;
        if (isGameOver) {
            tone(BUZZER, 100, 200); // Landing thud sound when game over
        }
    }

    // Move obstacle with variable speed
    obstacleX -= gameSpeed;
    if (obstacleX < -(useTree2 ? TREE2_WIDTH : TREE1_WIDTH)) {
        obstacleX = SCREEN_WIDTH;
        useTree2 = random(2) == 0;
        score++;
        tone(BUZZER, 1500, 50); // Score sound
    }

    // Check collision
    if (checkCollision()) {
        isGameOver = true;
        tone(BUZZER, 200, 300); // Crash sound
        delay(200);
        tone(BUZZER, 150, 300);
    }
}

bool TRexGame::checkCollision() {
    int dinoRight = DINO_WIDTH - 8;
    int dinoLeft = 4;
    int obstacleRight = obstacleX + (useTree2 ? TREE2_WIDTH : TREE1_WIDTH) - 2;
    int obstacleLeft = obstacleX + 2;
    
    if (obstacleLeft < dinoRight && obstacleRight > dinoLeft) {
        int dinoBottom = dinoY + DINO_HEIGHT - 2;
        int dinoTop = dinoY + 2;
        int obstacleBottom = GROUND_Y;
        int obstacleTop = GROUND_Y - (useTree2 ? TREE2_HEIGHT : TREE1_HEIGHT) + 2;
        
        if (dinoBottom > obstacleTop && dinoTop < obstacleBottom) {
            return true;
        }
    }
    return false;
}

void TRexGame::onJump() {
    if (!isJumping && !isGameOver) {
        dinoVelocity = JUMP_FORCE;
        isJumping = true;
        tone(BUZZER, 800, 50); // Jump sound
    }
}

void TRexGame::draw() {
    // Draw ground
    display->drawLine(0, GROUND_Y, SCREEN_WIDTH, GROUND_Y, SH110X_WHITE);
    
    // Draw dino
    display->drawBitmap(0, dinoY, dino1, DINO_WIDTH, DINO_HEIGHT, SH110X_WHITE);
    
    // Draw obstacle
    if (useTree2) {
        display->drawBitmap(obstacleX, GROUND_Y - TREE2_HEIGHT, tree2, TREE2_WIDTH, TREE2_HEIGHT, SH110X_WHITE);
    } else {
        display->drawBitmap(obstacleX, GROUND_Y - TREE1_HEIGHT, tree1, TREE1_WIDTH, TREE1_HEIGHT, SH110X_WHITE);
    }
    
    // Draw score
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(SCREEN_WIDTH - 70, 1);
    display->print("Score:");
    display->print(score);

    if (isGameOver) {
        display->setTextSize(1);
        display->setCursor(35, 30);
        display->print("GAME OVER!");
    }
}