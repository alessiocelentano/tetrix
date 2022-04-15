#include <time.h>
#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>

#define IPIECE         { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } }
#define SQUAREPIECE    { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }
#define LPIECE         { { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define LREVERSEDPIECE { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define ZPIECE         { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } }
#define ZREVERSEDPIECE { { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 } }
#define TPIECE         { { 0, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }

#define NOCOLOR        -1
#define CYAN           0
#define YELLOW         1
#define ORANGE         2
#define BLUE           3
#define RED            4
#define GREEN          5
#define PURPLE         6

#define SPAWNX         8
#define SPAWNY         0
#define UPLIMIT        0
#define LEFTLIMIT      0
#define RIGHTLIMIT     15
#define DOWNLIMIT      31
#define UP             129
#define LEFT           97
#define RIGHT          100
#define DOWN           115
#define LEFTPIN        11
#define RIGHTPIN       12
#define DOWNPIN        13
#define DELAY          200
#define INPUTDELAY     100
#define OFF            0
#define ON             1
#define XCOORDINATE    0
#define YCOORDINATE    1

#define CLK            8
#define LAT            10
#define OE             9
#define A              A0
#define B              A1
#define C              A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
int pieceBaseCoordinates[7][4][2] = { 
    IPIECE, SQUAREPIECE, LPIECE,
    LREVERSEDPIECE, ZPIECE,
    ZREVERSEDPIECE, TPIECE
};
char ledColorMatrix[16][32];
unsigned long currentTime;
unsigned long previousFallTime;
unsigned long color;
char colorID;
char x, y;

void setup() {
    Serial.begin(9600);
    matrix.begin();
    srand(time(0));
    previousFallTime = 0;
    currentTime = 0;
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        for (int positiony = UPLIMIT; positiony <= DOWNLIMIT; ++positiony)
            ledColorMatrix[positionx][positiony] = NOCOLOR;
}

void loop() {
    if (canFall()) {
        moveTo(x, y+1);
        previousFallTime = currentTime;
    } else {
        deleteFullLines();
        createNewPiece();
        if (isPositionAvailable(x, y)) drawNewPiece();
        else printGameOver();
    }
    while (!isFallDelayExpired()) {
        getInput();
        delay(INPUTDELAY);
    }
}

bool canFall() {
    if (previousFallTime == 0) return false;
    return isPositionAvailable(x, y+1);
}

void deleteFullLines() {
    for (int positiony = DOWNLIMIT; positiony > UPLIMIT; --positiony) {
        if (!isLineFull(positiony)) continue;
        deleteLine(positiony);
        dropLinesFrom(positiony);
    }
}

bool isLineFull(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        if (isAnActualAndTurnedOffPixel(positionx, positiony)) return false;
    return true;
}

void deleteLine(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        deletePixel(positionx, positiony);
}

void dropLinesFrom(int positiony) {
    while (positiony-- > UPLIMIT) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
            changeColorAndColorIDWith(positionx, positiony);
            dropPixelByOnePosition(positionx, positiony);
        }
    }
}

void changeColorAndColorIDWith(int positionx, int positiony) {
    colorID = ledColorMatrix[positionx][positiony-1];
    color = getColorByID(colorID);
}

void dropPixelByOnePosition(int positionx, int positiony) {
    deletePixel(positionx, positiony);
    ledColorMatrix[positionx][positiony] = NOCOLOR;
    drawPixel(positionx, positiony+1);
    ledColorMatrix[positionx][positiony+1] = colorID;
}

void createNewPiece() {
    x = SPAWNX;
    y = SPAWNY;
    colorID = rand() % 7;
    color = getColorByID(colorID);
}

unsigned int getColorByID(char color) {
    if (color == NOCOLOR) return matrix.Color333(0, 0, 0);
    if (color == CYAN) return matrix.Color333(0, 7, 7);
    if (color == YELLOW) return matrix.Color333(7, 7, 0);
    if (color == ORANGE) return matrix.Color333(7, 3, 0);
    if (color == BLUE) return matrix.Color333(0, 0, 7);
    if (color == RED) return matrix.Color333(7, 0, 0);
    if (color == GREEN) return matrix.Color333(0, 7, 0);
    if (color == PURPLE) return matrix.Color333(7, 0, 7);
}

bool isPositionAvailable(int positionx, int positiony) {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int newx = positionx + pieceBaseCoordinates[colorID][piecePixelIndex][XCOORDINATE];
        int newy = positiony + pieceBaseCoordinates[colorID][piecePixelIndex][YCOORDINATE];
        if (!isAnActualAndTurnedOffPixel(newx, newy) && !isACurrentPiecePixel(newx, newy))
            return false;
    }
    return true;
}

bool isAnActualAndTurnedOffPixel(int positionx, int positiony) {
    if (positionx < LEFTLIMIT || positionx > RIGHTLIMIT) return false;
    if (positiony > DOWNLIMIT) return false;
    if (ledColorMatrix[positionx][positiony] != NOCOLOR) return false;
    return true;
}

bool isACurrentPiecePixel(int newx, int newy) {
    /*  When this function is called ledColorMatrix is not updated yet.
     *  isAnActualAndTurnedOffPixel returns true even if the new piece
     *  collides with some old coordinates of the still not updated ledColorMatrix
     *  so we do another check
     */
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int currentx = x + pieceBaseCoordinates[colorID][piecePixelIndex][XCOORDINATE];
        int currenty = y + pieceBaseCoordinates[colorID][piecePixelIndex][YCOORDINATE];
        if (newx == currentx && newy == currenty) return true;
    }
    return false;
}

bool isFallDelayExpired() {
    currentTime = millis();
    int timeSinceLastFall = currentTime - previousFallTime;
    return timeSinceLastFall < DELAY;
}

void getInput() {
    int movement = Serial.read();
    if (digitalRead(UP)) hardDrop(x, y);
    if (digitalRead(LEFT)) moveTo(x-1, y);
    if (digitalRead(RIGHT)) moveTo(x+1, y);
    if (digitalRead(DOWN)) moveTo(x, y+1);
}

void hardDrop(int positionx, int positiony) {
    while (moveTo(positionx, ++positiony))
        ;
}

bool moveTo(int positionx, int positiony) {
    if (!isPositionAvailable(positionx, positiony)) return false;
    deleteCurrentPiece();
    updateCoordinates(positionx, positiony);
    drawNewPiece();
    return true;
}

void deleteCurrentPiece() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx = x + pieceBaseCoordinates[colorID][piecePixelIndex][XCOORDINATE];
        int positiony = y + pieceBaseCoordinates[colorID][piecePixelIndex][YCOORDINATE];
        deletePixel(positionx, positiony);
    }
}

void deletePixel(int positionx, int positiony) {
    unsigned int color = getColorByID(NOCOLOR);
    matrix.drawPixel(positiony, positionx, color);
    ledColorMatrix[positionx][positiony] = NOCOLOR;
}

void updateCoordinates(int positionx, int positiony) {
    x = positionx;
    y = positiony;
}

void drawNewPiece() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx = x + pieceBaseCoordinates[colorID][piecePixelIndex][XCOORDINATE];
        int positiony = y + pieceBaseCoordinates[colorID][piecePixelIndex][YCOORDINATE];
        drawPixel(positionx, positiony);
    }
}

void drawPixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, color);
    ledColorMatrix[positionx][positiony] = color;
}

void printGameOver() {
    int letterColor;
    for (int positionx = LEFTLIMIT; positionx < RIGHTLIMIT; ++positionx)
        for (int positiony = 9; positiony < 22; ++positiony)
            matrix.drawPixel(positiony, positionx, getColorByID(NOCOLOR));
    letterColor = getColorByID(RED);  // G
    matrix.drawLine(10, 12, 10, 14, letterColor);
    matrix.drawLine(11, 15, 13, 15, letterColor);
    matrix.drawLine(14, 14, 14, 12, letterColor);
    matrix.drawLine(13, 12, 12, 12, letterColor);
    matrix.drawPixel(12, 13, letterColor); 
    letterColor = getColorByID(ORANGE);  // A
    matrix.drawLine(11, 11, 14, 11, letterColor);
    matrix.drawLine(11, 9, 14, 9, letterColor);
    matrix.drawPixel(12, 10, letterColor);
    matrix.drawPixel(10, 10, letterColor);
    letterColor = getColorByID(YELLOW);  // M
    matrix.drawLine(10, 8, 14, 8, letterColor);
    matrix.drawLine(10, 4, 14, 4, letterColor);
    matrix.drawPixel(11, 7, letterColor);
    matrix.drawPixel(11, 5, letterColor);
    matrix.drawPixel(12, 6, letterColor);
    letterColor = getColorByID(GREEN);  // E
    matrix.drawLine(10, 3, 14, 3, letterColor);
    matrix.drawLine(10, 2, 10, 0, letterColor);
    matrix.drawLine(12, 2, 12, 1, letterColor);
    matrix.drawLine(14, 2, 14, 0, letterColor);
    letterColor = getColorByID(CYAN);  // O
    matrix.drawLine(16, 14, 16, 12, letterColor);
    matrix.drawLine(17, 15, 17, 19, letterColor);
    matrix.drawLine(17, 11, 19, 11, letterColor);
    matrix.drawLine(20, 14, 20, 12, letterColor);
    letterColor = getColorByID(BLUE);  // V
    matrix.drawLine(16, 10, 19, 10, letterColor);
    matrix.drawLine(16, 8, 19, 8, letterColor);
    matrix.drawPixel(20, 9, letterColor);
    letterColor = getColorByID(PURPLE);  // E
    matrix.drawLine(16, 7, 20, 7, letterColor);
    matrix.drawLine(16, 6, 16, 4, letterColor);
    matrix.drawLine(18, 6, 18, 5, letterColor);
    matrix.drawLine(20, 6, 20, 4, letterColor);
    letterColor = getColorByID(RED);  // R
    matrix.drawLine(16, 3, 16, 3, letterColor);
    matrix.drawLine(16, 2, 16, 1, letterColor);
    matrix.drawPixel(17, 1, letterColor);
    matrix.drawPixel(18, 2, letterColor);
    matrix.drawPixel(19, 1, letterColor);
    matrix.drawPixel(20, 0, letterColor);
}

/*
void printMatrixColors() {
    for (int positiony = UPLIMIT; positiony <= DOWNLIMIT; ++positiony) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
            Serial.print(ledColorMatrix[positionx][positiony], 10);
            Serial.print(" ");
        }
        Serial.println();
    }
}
/*

 */
