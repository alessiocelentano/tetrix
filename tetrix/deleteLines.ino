void deleteFullLines() {
    int linesDeleted[DOWNLIMIT+1] = { 0 };
    bool deleted = false;
    for (int positiony = DOWNLIMIT; positiony >= UPLIMIT; --positiony) {
        linesDeleted[positiony] = isLineFull(positiony);
        if (linesDeleted[positiony]) {
            deleteLine(positiony);
            deleted = true;
        }
    }
    if (deleted) {
        delay(DELETIONDELAY);
        for (int positiony = 0; positiony <= DOWNLIMIT; ++positiony) 
            if (linesDeleted[positiony]) dropLinesFrom(positiony);
    }
}

bool isLineFull(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        if (isAnActualAndTurnedOffPixel(positionx, positiony)) return false;
    return true;
}

void deleteLine(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
        matrix.drawPixel(positiony, positionx, NOCOLOR);
        updateLedColorMatrix(positionx, positiony, NOPIECE);
    }
}
                                           
void dropLinesFrom(int positiony) {
    while (--positiony > UPLIMIT) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
            pieceID = getPieceID(positionx, positiony);
            color = getColorByID(pieceID);
            dropPixel(positionx, positiony);
        }
    }
}

void dropPixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, NOCOLOR);
    matrix.drawPixel(positiony+1, positionx, color);
    updateLedColorMatrix(positionx, positiony, NOPIECE);
    updateLedColorMatrix(positionx, positiony+1, pieceID);
}
