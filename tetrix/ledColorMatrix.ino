unsigned long long getRow(char positiony) {
    unsigned long long byte1 = ((unsigned long long) ledColorMatrix[positiony][0]) << 2 * (sizeof(unsigned int) * 8);
    unsigned long long byte2 = ((unsigned long long) ledColorMatrix[positiony][1]) << sizeof(unsigned int) * 8;
    unsigned long long byte3 = ledColorMatrix[positiony][2];
    return byte1 | byte2 | byte3;
}

unsigned long long turnOnBits(char positionx, char positiony) {
    return getRow(positiony) & (0b111LL << ((RIGHTLIMIT - positionx)*3));
}

unsigned long long turnOffBits(char positionx, char positiony) {
    return getRow(positiony) & ~(0b111LL << ((RIGHTLIMIT - positionx)*3));
}

char getPieceID(int positionx, int positiony) {
    return turnOnBits(positionx, positiony) >> ((RIGHTLIMIT - positionx)*3);
}

bool isLedOn(char positionx, char positiony) {
    return (turnOnBits(positionx, positiony)) ? true: false;
}

void updateLedColorMatrix(int positionx, int positiony, int pieceID_) {
    unsigned long long row = turnOffBits(positionx, positiony) | ((unsigned long long) pieceID << (RIGHTLIMIT - positionx)*3);
    unsigned long long byte1 = row >> 2 * (sizeof(unsigned int) * 8);
    unsigned long long byte2 = (sizeof(unsigned int) * 8) & ~((unsigned int) 0);
    unsigned long long byte3 = row & ~((unsigned int) 0);
    ledColorMatrix[positiony][0] = byte1;
    ledColorMatrix[positiony][1] = byte2;
    ledColorMatrix[positiony][2] = byte3;
}
