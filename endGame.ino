void endGame() {
    deleteAllPieces();
    while (1) {
        printGameOver();
        unsigned long long previousTime = millis();
        while (millis() - previousTime < 1500) {
            controller.update();
            if (controller.cButton || controller.zButton) {
                setupNewGame();
                return;
            }
            delay(1);
        }
    }
}

void deleteAllPieces() {
    for (int positiony = UPLIMIT; positiony <= DOWNLIMIT; ++positiony)
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
            if (isLedOn(positionx, positiony)) {
                matrix.drawPixel(positiony, positionx, matrix.Color333(0, 0, 0));
                delay(50);
            }
}

void printGameOver() {
    clearScreen();
    matrix.drawLine(8, 15, 8, 0, WHITE);
    matrix.drawLine(22, 15, 22, 0, WHITE);
    delay(750);
    matrix.drawLine(10, 12, 10, 14, RED);
    matrix.drawLine(11, 15, 13, 15, RED);
    matrix.drawLine(14, 14, 14, 12, RED);
    matrix.drawLine(13, 12, 12, 12, RED);
    matrix.drawPixel(12, 13, RED);
    matrix.drawLine(11, 11, 14, 11, PURPLE);
    matrix.drawLine(11, 9, 14, 9, PURPLE);
    matrix.drawPixel(12, 10, PURPLE);
    matrix.drawPixel(10, 10, PURPLE);
    matrix.drawLine(10, 8, 14, 8, ORANGE);
    matrix.drawLine(10, 4, 14, 4, ORANGE);
    matrix.drawPixel(11, 7, ORANGE);
    matrix.drawPixel(11, 5, ORANGE);
    matrix.drawPixel(12, 6, ORANGE);
    matrix.drawLine(10, 3, 14, 3, BLUE);
    matrix.drawLine(10, 2, 10, 0, BLUE);
    matrix.drawLine(12, 2, 12, 1, BLUE);
    matrix.drawLine(14, 2, 14, 0, BLUE);
    matrix.drawLine(16, 14, 16, 12, YELLOW);
    matrix.drawLine(17, 15, 19, 15, YELLOW);
    matrix.drawLine(17, 11, 19, 11, YELLOW);
    matrix.drawLine(20, 14, 20, 12, YELLOW);
    matrix.drawLine(16, 10, 19, 10, CYAN);
    matrix.drawLine(16, 8, 19, 8, CYAN);
    matrix.drawPixel(20, 9, CYAN);
    matrix.drawLine(16, 7, 20, 7, GREEN);
    matrix.drawLine(16, 6, 16, 4, GREEN);
    matrix.drawLine(18, 6, 18, 5, GREEN);
    matrix.drawLine(20, 6, 20, 4, GREEN);
    matrix.drawLine(16, 3, 20, 3, RED);
    matrix.drawLine(16, 2, 16, 1, RED);
    matrix.drawPixel(17, 1, RED);
    matrix.drawPixel(18, 2, RED);
    matrix.drawPixel(19, 1, RED);
    matrix.drawPixel(20, 0, RED);
}
