#include "flappy_game.h"
#include "sprites.h"

FlappyGame::FlappyGame(Adafruit_SH1106G* disp) : display(disp) {
    pinMode(BUZZER, OUTPUT);
    wall_gap = 30;  // Match sample code
    wall_width = 10;
    init();
}

void FlappyGame::init() {
    birdY = display->height() / 2;
    birdVelocity = -4;  // Initial upward momentum like sample
    score = 0;
    isGameOver = false;

    // Initialize walls like sample code
    wall_x[0] = display->width();
    wall_y[0] = display->height() / 2 - wall_gap / 2;
    wall_x[1] = display->width() + display->width() / 2;
    wall_y[1] = display->height() / 2 - wall_gap / 2;

    // Play start sound
    tone(BUZZER, 1000, 100);
    delay(100);
    tone(BUZZER, 1500, 100);
}

void FlappyGame::update() {
    if(isGameOver) {
        // Kiểm tra nút IO25 để restart game
        if(digitalRead(25) == LOW) {
            init();
            delay(200); // Debounce
            return;
        }
        return;
    }

    // Update bird physics
    birdVelocity += GRAVITY;
    birdY += birdVelocity;

    // Keep bird on screen with waddle effect
    if(birdY < 0) {
        birdY = 0;
    }
    if(birdY > display->height() - SPRITE_HEIGHT) {
        birdY = display->height() - SPRITE_HEIGHT;
        birdVelocity = -2;  // Waddle effect from sample
        isGameOver = true;
        tone(BUZZER, 200, 300); // Crash sound
        delay(200);
        tone(BUZZER, 150, 300);
    }

    // Update walls
    for(int i = 0; i < 2; i++) {
        wall_x[i] -= WALL_SPEED;

        // Check collision with walls
        if((20 + SPRITE_WIDTH > wall_x[i] && 20 < wall_x[i] + wall_width) &&
           (birdY < wall_y[i] || birdY + SPRITE_HEIGHT > wall_y[i] + wall_gap)) {
            isGameOver = true;
            tone(BUZZER, 200, 300); // Crash sound
            delay(200);
            tone(BUZZER, 150, 300);
            return;
        }

        // Score point when passing wall
        if(wall_x[i] == 20) {
            score++;
            tone(BUZZER, 1500, 50); // Score sound
        }

        // Reset wall when it goes off screen
        if(wall_x[i] < 0) {
            wall_x[i] = display->width();
            wall_y[i] = random(0, display->height() - wall_gap);
        }
    }
}

void FlappyGame::onJump() {
    if(!isGameOver) {
        birdVelocity = JUMP_FORCE;
        tone(BUZZER, 800, 20); // Jump sound
    }
}

void FlappyGame::draw() {
    // Draw bird with animation like sample
    if(birdVelocity < 0) {
        if(random(2) == 0) {
            display->drawBitmap(20, birdY, wing_down_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SH110X_WHITE);
        } else {
            display->drawBitmap(20, birdY, wing_up_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SH110X_WHITE);
        }
    } else {
        display->drawBitmap(20, birdY, wing_up_bmp, SPRITE_WIDTH, SPRITE_HEIGHT, SH110X_WHITE);
    }

    // Draw walls
    for(int i = 0; i < 2; i++) {
        // Draw top wall
        display->fillRect(wall_x[i], 0, wall_width, wall_y[i], SH110X_WHITE);
        // Draw bottom wall
        display->fillRect(wall_x[i], wall_y[i] + wall_gap, wall_width, 
                       display->height() - wall_y[i] + wall_gap, SH110X_WHITE);
    }

    // Draw score
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(1, 1);
    display->print("Score:");
    display->print(score);

    if(isGameOver) {
        display->setTextSize(1);
        display->setCursor(25, 20);
        display->print("Game Over!");
        display->setCursor(10, 30);
        display->print("Press to restart");
    }
    
    // Apply game speed delay
    if(!isGameOver) {
        delay(GAME_SPEED);
    }
}