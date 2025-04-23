#include "starwar_game.h"

// Game bitmap data
const unsigned char PROGMEM StarWarGame::dioda16[] = {
    0x00, 0x00, 0x00, 0x00, 0x1C, 0x00, 0x3F, 0xF0, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0x7F, 0xFF,
    0x7F, 0xFF, 0xFF, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x1F, 0xF0, 0x1C, 0x00, 0x00, 0x00, 0x00, 0x00
};

StarWarGame::StarWarGame(Adafruit_SH1106G* disp) : display(disp) {
    // Initialize pins
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SHOOT, INPUT_PULLUP);
    pinMode(BTN_MENU, INPUT_PULLUP);
    pinMode(BUZZER, OUTPUT);
}

void StarWarGame::beep(int note, int duration) {
    tone(BUZZER, note, duration);
    noTone(BUZZER);
}

void StarWarGame::ponovo() {
    metx = 0; mety = 0; postoji = 0;
    nep = 8; smjer = 0; go = 0;
    rx = 95; ry = 0;
    rx2 = 95; ry2 = 0;
    rx3 = 95; ry3 = 0;
    bodovi = 0; brzina = 3;
    bkugle = 1;
    najmanja = 600; najveca = 1200;
    promjer = 12;
    rx4 = 95; ry4 = 0;
    zivoti = 5;
    poc = 0; ispaljeno = 0;
    nivo = 1;
    pocetno = 0; odabrano = 0;
    trenutno = 0; nivovrije = 0;
    poz = 30;
    centar = 95;
}

void StarWarGame::init() {
    ponovo();
    isRunning = true;
    go = 0;

    // Show welcome screen without blocking delay
    display->clearDisplay();
    display->setTextSize(1);
    display->setCursor(10, 10);
    display->println("Star Wars Game");
    display->setCursor(10, 30);
    display->println("Death Star");
    display->setCursor(10, 40);
    display->println("vs X-Wing");
    display->display();
    
    // Short beep instead of tune
    beep(440, 100);
}

void StarWarGame::update() {
    // Kiểm tra nút BTN_MENU cho chức năng về menu
    static unsigned long menuButtonPressTime = 0;
    static bool menuButtonPressed = false;

    if (!digitalRead(BTN_MENU)) {
        if (!menuButtonPressed) {
            menuButtonPressed = true;
            menuButtonPressTime = millis();
        }
        // Nếu giữ nút 1 giây thì về menu
        if (millis() - menuButtonPressTime >= 1000) {
            isRunning = false;
            return;
        }
    } else {
        menuButtonPressed = false;
    }

    if (go == 0) {
        if (poc == 0) {
            pocetno = millis();
            odabrano = random(400, 1200);
            poc = 1;
        }
        trenutno = millis();

        // Level management
        if ((trenutno - nivovrije) > 50000) {
            nivovrije = trenutno;
            nivo = nivo + 1;
            brzina = brzina + 1;
            if (nivo % 2 == 0) {
                bkugle = bkugle + 1;
                promjer = promjer - 1;
            }
            najmanja = najmanja - 50;
            najveca = najveca - 50;
        }

        // Enemy bullet management
        if ((odabrano + pocetno) < trenutno) {
            poc = 0;
            ispaljeno = ispaljeno + 1;
            if (ispaljeno == 1) {
                rx = 95;
                ry = nep;
            }
            if (ispaljeno == 2) {
                rx2 = 95;
                ry2 = nep;
            }
            if (ispaljeno == 3) {
                rx3 = 95;
                ry3 = nep;
            }
            if (ispaljeno == 4) {
                rx4 = 95;
                ry4 = nep;
            }
        }

        // Player movement
        if (!digitalRead(BTN_UP) && poz >= 2) {
            poz = poz - 2;
        }
        if (!digitalRead(BTN_DOWN) && poz <= 46) {
            poz = poz + 2;
        }

        // Player shooting
        if (!digitalRead(BTN_SHOOT) && postoji == 0) {
            postoji = 1;
            metx = 6;
            mety = poz + 8;
            tone(BUZZER, 1200, 20);
        }

        if (postoji == 1) {
            metx = metx + 8;
        }

        // Reset player bullet
        if (metx > 128) postoji = 0;

        // Move enemy
        if (smjer == 0) {
            nep = nep + bkugle;
        } else {
            nep = nep - bkugle;
        }

        if (nep >= (64-promjer)) smjer = 1;
        if (nep <= promjer) smjer = 0;

        // Check for hits
        if (mety >= nep-promjer && mety <= nep+promjer) {
            if (metx > (centar-promjer) && metx < (centar+promjer)) {
                metx = -20;
                tone(BUZZER, 500, 20);
                bodovi = bodovi + 1;
                postoji = 0;
            }
        }

        // Calculate player position (pozicija) before collision detection
        int pozicija = poz + 8;

        // Update bullet positions and check collisions
        if (ispaljeno > 0) {
            rx = rx - brzina;
            if (ry >= pozicija-8 && ry <= pozicija+8 && rx < 12 && rx > 4) {
                rx = 95;
                ry = -50;
                tone(BUZZER, 100, 100);
                zivoti--;
            }
        }
        
        if (ispaljeno > 1) {
            rx2 = rx2 - brzina;
            if (ry2 >= pozicija-8 && ry2 <= pozicija+8 && rx2 < 12 && rx2 > 4) {
                rx2 = -50;
                ry2 = -50;
                tone(BUZZER, 100, 100);
                zivoti--;
            }
        }
        
        if (ispaljeno > 2) {
            rx3 = rx3 - brzina;
            if (ry3 >= pozicija-8 && ry3 <= pozicija+8 && rx3 < 12 && rx3 > 4) {
                rx3 = -50;
                ry3 = -50;
                tone(BUZZER, 100, 100);
                zivoti--;
            }
        }
        
        if (ispaljeno > 3) {
            rx4 = rx4 - brzina;
            if (ry4 >= pozicija-8 && ry4 <= pozicija+8 && rx4 < 12 && rx4 > 4) {
                rx4 = 200;
                ry4 = -50;
                ispaljeno = 0;
                tone(BUZZER, 100, 100);
                zivoti--;
            }
        }

        // Check for out of bounds bullets
        if (rx4 < 1) {
            ispaljeno = 0;
            rx4 = 200;
        }

        if (zivoti == 0) go = 1;
    }
}

void StarWarGame::draw() {
    display->clearDisplay();

    if (go == 0) {
        // Draw background stars
        display->drawPixel(50, 30, SH110X_WHITE);
        display->drawPixel(30, 17, SH110X_WHITE);
        display->drawPixel(60, 18, SH110X_WHITE);
        display->drawPixel(55, 16, SH110X_WHITE);
        display->drawPixel(25, 43, SH110X_WHITE);

        // Draw enemy bullets
        if (ispaljeno > 0) {
            display->drawCircle(rx, ry, 2, SH110X_WHITE);
        }
        if (ispaljeno > 1) {
            display->drawCircle(rx2, ry2, 1, SH110X_WHITE);
        }
        if (ispaljeno > 2) {
            display->drawCircle(rx3, ry3, 4, SH110X_WHITE);
        }
        if (ispaljeno > 3) {
            display->drawCircle(rx4, ry4, 2, SH110X_WHITE);
        }

        // Draw player bullet
        if (postoji == 1) {
            display->drawLine(metx, mety, metx + 4, mety, SH110X_WHITE);
        }

        // Draw player ship
        display->drawBitmap(4, poz, dioda16, 16, 16, SH110X_WHITE);
        
        // Draw enemy (Death Star)
        display->fillCircle(centar, nep, promjer, SH110X_WHITE);
        display->fillCircle(centar + 2, nep + 3, promjer/3, SH110X_BLACK);

        // Display score and stats
        display->setTextSize(1);
        display->setCursor(33, 57);
        display->print("Score:");
        display->setCursor(68, 57);
        display->print(bodovi);

        display->setCursor(33, 0);
        display->print("Lives:");
        display->setCursor(68, 0);
        display->print(zivoti);

        display->setCursor(110, 0);
        display->print("L:");
        display->setCursor(122, 0);
        display->print(nivo);
    } else {
        // Game Over screen
        if (zivoti == 0) {
            tone(BUZZER, 200, 300);
            delay(300);
            tone(BUZZER, 250, 200);
            delay(200);
            tone(BUZZER, 300, 300);
            delay(300);
            zivoti = 5;
        }

        display->setTextSize(2);
        display->setCursor(7, 10);
        display->println("GAME OVER!");
        display->setTextSize(1);
        display->setCursor(7, 30);
        display->print("Score: ");
        display->println(bodovi);
        display->setCursor(7, 40);
        display->print("Level: ");
        display->println(nivo);
        display->setCursor(7, 50);
        display->print("Time(s): ");
        display->println(trenutno/1000);

        if (!digitalRead(BTN_SHOOT)) {
            tone(BUZZER, 280, 300);
            delay(300);
            tone(BUZZER, 250, 200);
            delay(200);
            tone(BUZZER, 370, 300);
            delay(300);
            ponovo();
            go = 0;
        }
    }

    display->display();
}