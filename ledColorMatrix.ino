unsigned long long getRow(char y2) {
    unsigned long long byte1 = ((unsigned long long) ledColorMatrix[y2][0]) << 32;
    unsigned long long byte2 = ((unsigned long long) ledColorMatrix[y2][1]) << 16;
    unsigned long long byte3 = ledColorMatrix[y2][2];
    return byte1 | byte2 | byte3;
}

bool isLedOn(char x2, char y2) {
    unsigned long long row = getRow(y2);
    return row & (0b111LL << ((RIGHTLIMIT - x2)*3)) ? true: false;
}

void turnOn(int x2, int y2) {
    unsigned long long row = ((getRow(y2) & ~(0b111LL << ((RIGHTLIMIT - x2)*3))) | (unsigned long long) pieceID << (RIGHTLIMIT - x2)*3);
    unsigned long long byte1 = row >> 32;
    unsigned long long byte2 = (row >> 16) & 0b1111111111111111;
    unsigned long long byte3 = row & 0b1111111111111111;
    ledColorMatrix[y2][0] = byte1;
    ledColorMatrix[y2][1] = byte2;
    ledColorMatrix[y2][2] = byte3;
}

void turnOff(char x2, char y2) {
    unsigned long long row = getRow(y2) & ~(0b111LL << ((RIGHTLIMIT - x2)*3));
    unsigned long long byte1 = row >> 32;
    unsigned long long byte2 = (row >> 16) & 0b1111111111111111;
    unsigned long long byte3 = row & 0b1111111111111111;
    ledColorMatrix[y2][0] = byte1;
    ledColorMatrix[y2][1] = byte2;
    ledColorMatrix[y2][2] = byte3;
}
