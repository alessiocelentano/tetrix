void deleteFullLines() {
    int linesDeleted[DOWNLIMIT+1] = { 0 };
    bool del = false;
    for (int positiony = DOWNLIMIT; positiony >= UPLIMIT; --positiony) {
        if (!isLineFull(positiony)) {
            linesDeleted[positiony] = 0;
            continue;
        }
        deleteLine(positiony);
        linesDeleted[positiony] = 1;
        del = true;
    }
    if (del) delay(DELETIONDELAY);
    for (int i = 0; i <= DOWNLIMIT; ++i) 
        if (linesDeleted[i]) dropLinesFrom(i);
}

bool isLineFull(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        if (isAnActualAndTurnedOffPixel(positionx, positiony)) return false;
    return true;
}

void deleteLine(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
        matrix.drawPixel(positiony, positionx, NOCOLOR);
        turnOff(positionx, positiony);
    }
}
                                           
void dropLinesFrom(int positiony) {
    while (--positiony > UPLIMIT) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
            changePieceIdAndColorWith(positionx, positiony);
            dropPixelByOnePosition(positionx, positiony);
        }
    }
}

void changePieceIdAndColorWith(int positionx, int positiony) {
    pieceID = (getRow(positiony) & (0b111LL << ((RIGHTLIMIT - positionx)*3))) >> ((RIGHTLIMIT - positionx)*3);
    color = getColorByID(pieceID);
}

void dropPixelByOnePosition(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, NOCOLOR);
    matrix.drawPixel(positiony+1, positionx, color);
    turnOff(positionx, positiony);
    if (color) turnOn(positionx, positiony+1);
}
