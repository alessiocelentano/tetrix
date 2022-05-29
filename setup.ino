void setupNewGame() {
    clearScreen();
    clearLedColorMatrix();
    leftLock = true;
    rightLock = true;
    downLock = true;
    cButtonLock = true;
    zButtonLock = true;
}

void setupNewTurn() {
    updateLedColorMatrix();
    deleteFullLines();
    setupNewPiece();
    if (isPositionAvailable(SPAWNX, SPAWNY, SPAWNROTATION)) {
        createShadow();
        drawNewPiece(SPAWNX, SPAWNY, color);
    } else {
        endGame();
    }
}

void setupNewPiece() {
    x = SPAWNX;
    y = SPAWNY;
    shadowx = SPAWNX;
    shadowy = SPAWNY;
    rotationState = 0;
    pieceID = (random() % 7) + 1;
    color = getColorByID(pieceID);
}

void clearScreen() {
    for (int positiony = UPLIMIT; positiony <= DOWNLIMIT; ++positiony)
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
            matrix.drawPixel(positiony, positionx, NOCOLOR);
}

void clearLedColorMatrix() {
    for (int positiony = UPLIMIT; positiony <= DOWNLIMIT; ++positiony) {
        ledColorMatrix[positiony][0] = NOPIECE;
        ledColorMatrix[positiony][1] = NOPIECE;
        ledColorMatrix[positiony][2] = NOPIECE;
    }
}

void updateLedColorMatrix() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx, positiony;
        getCoordinates(x, y, &positionx, &positiony, rotationState, piecePixelIndex);
        turnOn(positionx, positiony);
    }
}

unsigned int getColorByID(char pieceID) {
    if (pieceID == NOPIECE) return NOCOLOR;
    if (pieceID == I_ID) return CYAN;
    if (pieceID == SQUARE_ID) return YELLOW;
    if (pieceID == REVERSED_L_ID) return ORANGE;
    if (pieceID == L_ID) return BLUE;
    if (pieceID == REVERSED_Z_ID) return RED;
    if (pieceID == Z_ID) return GREEN;
    if (pieceID == T_ID) return PURPLE;
}
