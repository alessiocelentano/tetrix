bool canFall() {
    if (!isPositionAvailable(x, y+1, rotationState)) {
        getInput(PLACEMENTDELAY);
        if (!isPositionAvailable(x, y+1, rotationState)) return false;
    }
    return true;
}

void deleteCurrentPiece() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx, positiony;
        getCoordinates(x, y, &positionx, &positiony, rotationState, piecePixelIndex);
        matrix.drawPixel(positiony, positionx, NOCOLOR);
    }
}

void getCoordinates(int positionx, int positiony, int* newx, int* newy, int rotationState, int piecePixelIndex) {
    int baseCoordinateX = basePieceCoordinates[pieceID-1][piecePixelIndex][XCOORDINATE];
    int baseCoordinateY = basePieceCoordinates[pieceID-1][piecePixelIndex][YCOORDINATE];
    rotationState %= 4;
    for (int i = 0; i < rotationState; ++i) {
        int tmp = baseCoordinateY;
        baseCoordinateY = baseCoordinateX;
        baseCoordinateX = -tmp;
    }
    *newx = positionx + baseCoordinateX;
    *newy = positiony + baseCoordinateY;
}

void createShadow() {
    shadowx = x;
    shadowy = y;
    while (isPositionAvailable(shadowx, ++shadowy, rotationState))
        ;
    drawNewPiece(shadowx, --shadowy, WHITE);
}

void deleteShadow() {
    drawNewPiece(shadowx, shadowy, NOPIECE);
}

bool isPositionAvailable(int positionx, int positiony, int rotationState) {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int newx, newy;
        getCoordinates(positionx, positiony, &newx, &newy, rotationState, piecePixelIndex);
        if (!isAnActualAndTurnedOffPixel(newx, newy)) return false;
    }
    return true;
}

bool isAnActualAndTurnedOffPixel(int positionx, int positiony) {
    if (positionx < LEFTLIMIT || positionx > RIGHTLIMIT) return false;
    if (positiony > DOWNLIMIT) return false;
    if (isLedOn(positionx, positiony)) return false;
    return true;
}

void drawNewPiece(int x2, int y2, int pieceColor) {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx, positiony;
        getCoordinates(x2, y2, &positionx, &positiony, rotationState, piecePixelIndex);
        matrix.drawPixel(positiony, positionx, pieceColor);
    }
}
