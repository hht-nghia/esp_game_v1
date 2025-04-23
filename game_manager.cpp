#include "game.h"
#include "trex_game.h"
#include "tictactoe_game.h"
#include "flappy_game.h"
#include "animation_game.h"
#include "starwar_game.h"

#define LONG_PRESS_DURATION 1000 // 1 second for long press
#define DEBOUNCE_DELAY 200      // 200ms for debounce
#define MOVE_DELAY 300          // 300ms for movement delay

GameManager::GameManager(Adafruit_SH1106G* disp) : 
    display(disp),
    currentState(SPLASH_SCREEN),
    menuSelection(0),
    buttonPressStartTime(0),
    lastButtonPressTime(0),
    isLongPress(false) {
    trexGame = new TRexGame(display);
    tictactoeGame = new TicTacToeGame(display);
    flappyGame = new FlappyGame(display);
    animationGame = new AnimationGame(display);
    starWarGame = new StarWarGame(display);
}

GameManager::~GameManager() {
    delete trexGame;
    delete tictactoeGame;
    delete flappyGame;
    delete animationGame;
    delete starWarGame;
}

void GameManager::init() {
    pinMode(BUTTON_MOVE, INPUT_PULLUP);
    pinMode(BUTTON_ACTION, INPUT_PULLUP);
    pinMode(BATTERY_PIN, INPUT); // Initialize battery monitoring pin
    analogReadResolution(12); // Set ADC resolution to 12 bits for ESP32
    currentState = SPLASH_SCREEN;
    menuSelection = 0;
    trexGame->init();
    tictactoeGame->init();
    flappyGame->init();
    animationGame->init();
    starWarGame->init();
}

int GameManager::getBatteryPercentage() {
    // Read raw ADC value
    int adcValue = analogRead(BATTERY_PIN);
    
    // Convert to actual battery voltage (after voltage divider)
    float measuredV = ((adcValue / 4095.0) * 3.3) * ADC_SCALE; 
    
    // Map voltage range to percentage (3.3V = 0%, 4.2V = 100%)
    float percentFloat = map(measuredV * 100, 
                           BAT_MIN_V * 100, 
                           BAT_MAX_V * 100, 
                           0, 
                           100);
    
    // Constrain to valid range
    percentFloat = constrain(percentFloat, 0.0, 100.0);
    
    // Debug output
    Serial.printf("ADC: %d, Voltage: %.3fV, Battery: %.1f%%\n", 
                 adcValue, measuredV, percentFloat);
    
    return (int)percentFloat;
}

void GameManager::update() {
    handleInput();
    
    switch (currentState) {
        case SPLASH_SCREEN:
            if (digitalRead(BUTTON_ACTION) == LOW) {
                currentState = MENU;
                delay(200); // Debounce
            }
            break;
            
        case MENU:
            handleMenu();
            break;
            
        case PLAYING_TREX:
            trexGame->update();
            break;
            
        case PLAYING_TICTACTOE:
            tictactoeGame->update();
            break;
            
        case PLAYING_FLAPPY:
            flappyGame->update();
            break;
            
        case PLAYING_ANIMATION:
            animationGame->update();
            // Check for long press to return to menu
            if (digitalRead(BUTTON_ACTION) == LOW) {
                if (!isLongPress) {
                    buttonPressStartTime = millis();
                    isLongPress = true;
                } else if (millis() - buttonPressStartTime >= 1000) {
                    currentState = MENU;
                    isLongPress = false;
                    delay(200);
                }
            } else {
                isLongPress = false;
            }
            break;

        case PLAYING_STARWAR:
            starWarGame->update();
            if (!starWarGame->isGameRunning()) {
                currentState = MENU;
            }
            break;
    }
}

void GameManager::draw() {
    display->clearDisplay();
    
    switch (currentState) {
        case SPLASH_SCREEN:
            drawSplashScreen();
            break;
            
        case MENU:
            drawMenu();
            break;
            
        case PLAYING_TREX:
            trexGame->draw();
            break;
            
        case PLAYING_TICTACTOE:
            tictactoeGame->draw();
            break;
            
        case PLAYING_FLAPPY:
            flappyGame->draw();
            break;
            
        case PLAYING_ANIMATION:
            animationGame->draw();
            break;

        case PLAYING_STARWAR:
            starWarGame->draw();
            break;
    }
    
    display->display();
}

void GameManager::drawSplashScreen() {
    display->setTextSize(2);
    display->setTextColor(SH110X_WHITE);
    display->setCursor(15, 20);
    display->println("SABO GAME");
    
    // Draw battery percentage in top right corner
    display->setTextSize(1);
    int batteryLevel = getBatteryPercentage();
    display->setCursor(100, 0);
    display->print(batteryLevel);
    display->print("%");
    
    // Draw battery icon
    display->drawRect(80, 0, 12, 8, SH110X_WHITE);     // Main battery outline
    display->fillRect(92, 2, 2, 4, SH110X_WHITE);      // Battery positive terminal
    int fillWidth = map(batteryLevel, 0, 100, 0, 10);
    display->fillRect(81, 1, fillWidth, 6, SH110X_WHITE);
    
    // Display measured voltage
    // int adcValue = analogRead(BATTERY_PIN);
    // float measuredV = ((adcValue / 4095.0) * 3.3) * ADC_SCALE;
    // display->setCursor(1, 10); // Adjust position as needed
    // display->print("Voltage: ");
    // display->print(measuredV, 2); // Display with 2 decimal places
    // display->print("V");
    
    // Draw "Press Enter to start" message
    display->setCursor(1, 45);
    display->println("Press Enter to start");
}

void GameManager::drawMenu() {
    display->clearDisplay();
    display->setTextSize(1);
    display->setTextColor(SH110X_WHITE);
    
    const int MENU_ITEMS = 5;  // Total number of menu items
    const int VISIBLE_ITEMS = 4;  // Number of items visible at once
    const int ITEM_HEIGHT = 12;  // Height of each menu item
    
    // Calculate scroll position
    int startIndex = max(0, min(menuSelection - 1, MENU_ITEMS - VISIBLE_ITEMS));
    
    // Draw battery percentage in top right corner
    display->setTextSize(1);
    int batteryLevel = getBatteryPercentage();
    display->setCursor(100, 0);
    display->print(batteryLevel);
    display->print("%");
    
    // Draw battery icon
    display->drawRect(80, 0, 12, 8, SH110X_WHITE);     // Main battery outline
    display->fillRect(92, 2, 2, 4, SH110X_WHITE);      // Battery positive terminal
    int fillWidth = map(batteryLevel, 0, 100, 0, 10);
    display->fillRect(81, 1, fillWidth, 6, SH110X_WHITE);

    // Draw title
    display->setCursor(1, 0);
    display->println("SELECT GAME");
    
    // Draw menu items with scroll
    const char* menuItems[] = {"T-Rex Run", "Tic Tac Toe", "Flappy Bird", "Animation", "Star Wars"};
    for (int i = 0; i < VISIBLE_ITEMS && (i + startIndex) < MENU_ITEMS; i++) {
        display->setCursor(10, 15 + i * ITEM_HEIGHT);
        if (i + startIndex == menuSelection) {
            display->print("> ");
        } else {
            display->print("  ");
        }
        display->println(menuItems[i + startIndex]);
    }
    
    // Draw scroll indicators if needed
    if (startIndex > 0) {
        display->drawTriangle(120, 15, 123, 20, 126, 15, SH110X_WHITE);  // Up arrow
    }
    if (startIndex + VISIBLE_ITEMS < MENU_ITEMS) {
        display->drawTriangle(120, 55, 123, 50, 126, 55, SH110X_WHITE);  // Down arrow
    }
    
    display->display();
}

void GameManager::handleMenu() {
    if (digitalRead(BUTTON_MOVE) == LOW) {
        menuSelection = (menuSelection + 1) % 5;  // Now 5 options
        delay(200); // Debounce
    }
    
    if (digitalRead(BUTTON_ACTION) == LOW) {
        switch(menuSelection) {
            case 0:
                currentState = PLAYING_TREX;
                trexGame->init();
                break;
            case 1:
                currentState = PLAYING_TICTACTOE;
                tictactoeGame->init();
                break;
            case 2:
                currentState = PLAYING_FLAPPY;
                flappyGame->init();
                break;
            case 3:
                currentState = PLAYING_ANIMATION;
                animationGame->init();
                break;
            case 4:
                currentState = PLAYING_STARWAR;
                starWarGame->init();
                break;
        }
        delay(200); // Debounce
    }
}

void GameManager::handleInput() {
    unsigned long currentTime = millis();
    bool movePressed = digitalRead(BUTTON_MOVE) == LOW;
    bool actionPressed = digitalRead(BUTTON_ACTION) == LOW;
    
    // Check for return to menu (long press IO26)
    if (actionPressed && buttonPressStartTime == 0) {
        buttonPressStartTime = currentTime;
    }
    
    if (!actionPressed) {
        if (buttonPressStartTime > 0 && (currentTime - buttonPressStartTime) >= LONG_PRESS_DURATION) {
            currentState = MENU;
            delay(DEBOUNCE_DELAY);
        }
        buttonPressStartTime = 0;
    }

    switch(currentState) {
        case PLAYING_TREX:
            if (movePressed) {
                trexGame->onJump();
            }
            break;
            
        case PLAYING_TICTACTOE:
            if (movePressed) {
                if (currentTime - lastButtonPressTime >= MOVE_DELAY) {
                    tictactoeGame->moveSelector();
                    lastButtonPressTime = currentTime;
                }
            }
            if (actionPressed && !isLongPress) {
                tictactoeGame->makeMove();
            }
            break;
            
        case PLAYING_FLAPPY:
            if (movePressed) {
                flappyGame->onJump();
            }
            break;
    }
    
    // Check if we need to return to menu
    if (actionPressed && (currentTime - buttonPressStartTime) >= LONG_PRESS_DURATION) {
        currentState = MENU;
        delay(DEBOUNCE_DELAY);
    }
}

bool GameManager::checkLongPress() {
    return buttonPressStartTime > 0 && 
           (millis() - buttonPressStartTime) >= LONG_PRESS_DURATION;
}