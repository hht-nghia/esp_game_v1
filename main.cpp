#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include "game.h"

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_SDA 19
#define OLED_SCL 18
#define OLED_RESET -1
#define i2c_Address 0x3c

// Initialize display
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Game manager instance
GameManager gameManager(&display);

void setup() {
    // Initialize display
    Wire.begin(OLED_SDA, OLED_SCL);
    display.begin(i2c_Address, true);
    display.clearDisplay();
    display.display();

    // Initialize game manager
    gameManager.init();
}

void loop() {
    // Update game state
    gameManager.update();

    // Draw game
    gameManager.draw();

    delay(20); // Control refresh rate
}