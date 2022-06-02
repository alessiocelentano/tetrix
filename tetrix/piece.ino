bool canFall() {
    if (!isPositionAvailable(x, y+1)) {
        getInput(PLACEMENTDELAY);
        if (!isPositionAvailable(x, y+1)) return false;
    }
    return true;
}

bool isPositionAvailable(int x_, int y_) {
    for (int pieceIndex = 0; pieceIndex < 4; ++pieceIndex) {
        int positionx, positiony;
        getCoordinates(x_, y_, &positionx, &positiony, pieceIndex);
        if (!isAnActualAndTurnedOffPixel(positionx, positiony)) return false;
    }
    return true;
}

void getCoordinates(int x_, int y_, int* positionx, int* positiony, int pieceIndex) {
    rotationState %= 4;
    int baseCoordinateX = basePieceCoordinates[pieceID-1][pieceIndex][XCOORDINATE];
    int baseCoordinateY = basePieceCoordinates[pieceID-1][pieceIndex][YCOORDINATE];
    for (int i = 0; i < rotationState; ++i) {
        int tmp = baseCoordinateY;
        baseCoordinateY = baseCoordinateX;
        baseCoordinateX = -tmp;
    }
    *positionx = x_ + baseCoordinateX;
    *positiony = y_ + baseCoordinateY;
}

bool isAnActualAndTurnedOffPixel(int positionx, int positiony) {
    if (positionx < LEFTLIMIT || positionx > RIGHTLIMIT) return false;
    if (positiony > DOWNLIMIT) return false;
    if (isLedOn(positionx, positiony)) return false;
    return true;
}

void createShadow() {
    shadowx = x;
    shadowy = y;
    while (isPositionAvailable(shadowx, ++shadowy));
    // shadowy has to be decreased because we have checked that
    // that position is not available with the while condition
    drawNewPiece(shadowx, --shadowy, WHITE);
}

void deleteCurrentPiece() {
    drawNewPiece(x, y, NOPIECE);
}

void deleteShadow() {
    drawNewPiece(shadowx, shadowy, NOPIECE);
}

void drawNewPiece(int x_, int y_, int pieceColor) {
    for (int pieceIndex = 0; pieceIndex < 4; ++pieceIndex) {
        int positionx, positiony;
        getCoordinates(x_, y_, &positionx, &positiony, pieceIndex);
        matrix.drawPixel(positiony, positionx, pieceColor);
    }
}
