#include <time.h>
#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>

#define IPIECE         { { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 } }
#define SQUAREPIECE    { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } }
#define LPIECE         { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define LREVERSEDPIECE { { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define ZPIECE         { { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 } }
#define ZREVERSEDPIECE { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } }
#define TPIECE         { { 0, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define LIGHTBLUE      matrix.Color333(0, 7, 7)
#define YELLOW         matrix.Color333(7, 7, 0)
#define ORANGE         matrix.Color333(7, 3, 0)
#define BLUE           matrix.Color333(0, 0, 7)
#define RED            matrix.Color333(7, 0, 0)
#define GREEN          matrix.Color333(0, 7, 0)
#define PURPLE         matrix.Color333(7, 0, 7)
#define NOCOLOR        matrix.Color333(0, 0, 0)
   
#define SPAWNX         8
#define SPAWNY         0
#define UPLIMIT        0
#define LEFTLIMIT      0
#define RIGHTLIMIT     15
#define DOWNLIMIT      31
#define UP             0
#define LEFT           1
#define RIGHT          2
#define DOWN           3
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
short pieceBaseCoordinates[7][4][2] = { 
    IPIECE, SQUAREPIECE, LPIECE,
    LREVERSEDPIECE, ZPIECE,
    ZREVERSEDPIECE, TPIECE
};
unsigned short colors[7] = {
    LIGHTBLUE, YELLOW, ORANGE,
    BLUE, RED, GREEN, PURPLE
};
unsigned short ledColorMatrix[16][32] = {NOCOLOR};
bool isNewPieceSpawnable = true;
unsigned int previousFallTime = 0;
unsigned int pieceColor;
char pieceId;
char x, y;

void setup() {
    matrix.begin();
    Serial.begin(9600);
    srand(time(0));
}

void loop() {
    if (isNewPieceSpawnable) {
        deleteFullLines();
        createNewPiece();
        isNewPieceSpawnable = false;
    }
    while (!isFallTimerExpired()) {
        getInput();
        delay(INPUTDELAY);
    }
    fall();
    previousFallTime = millis();
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
        if (!isAnActualAndTurnedOffPixel(positionx, positiony)) return false;
    return true;
}

bool isAnActualAndTurnedOffPixel(int positionx, int positiony) {
    if (positionx < LEFTLIMIT || positionx > RIGHTLIMIT) return false;
    if (positiony > DOWNLIMIT) return false;
    if (ledColorMatrix[positionx][positiony] != NOCOLOR) return false;
    return true;
}

void deleteLine(int positiony) {
    for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
        deletePixel(positionx, positiony);
}

void deletePixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, NOCOLOR);
    ledColorMatrix[positionx][positiony] = NOCOLOR;
}

void dropLinesFrom(int positiony) {
    while (positiony-- > UPLIMIT) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx) {
            makePieceColorTheDroppingPixelColor(positionx, positiony);
            dropPixelByOnePosition(positionx, positiony);
        }
    }
}

void makePieceColorTheDroppingPixelColor(int positionx, int positiony) {
    pieceColor = ledColorMatrix[positionx][positiony];
}

void dropPixelByOnePosition(int positionx, int positiony) {
    deletePixel(positionx, positiony);
    ledColorMatrix[positionx][positiony] = NOCOLOR;
    drawPixel(positionx, positiony+1);
    ledColorMatrix[positionx][positiony+1] = pieceColor;
}

void drawPixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, pieceColor);
    ledColorMatrix[positionx][positiony] = pieceColor;
}



void createNewPiece() {
    x = SPAWNX;
    y = SPAWNY;
    pieceId = rand() % 7;
    pieceColor = colors[pieceId];
    drawNewPiece();
}

void drawNewPiece() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx = x + pieceBaseCoordinates[pieceId][piecePixelIndex][XCOORDINATE];
        int positiony = y + pieceBaseCoordinates[pieceId][piecePixelIndex][YCOORDINATE];
        drawPixel(positionx, positiony);
    }
}

bool isFallTimerExpired() {
    int timeSinceLastFall = millis() - previousFallTime;
    return timeSinceLastFall < DELAY;
}

void getInput() {
    // if (digitalRead(UPPIN)) hardDrop();
    if (digitalRead(LEFTPIN)) moveTo(LEFT);
    if (digitalRead(RIGHTPIN)) moveTo(RIGHT);
    if (digitalRead(DOWNPIN)) moveTo(DOWN);
}

void hardDrop() {
    while (moveTo(DOWN))
        ;
}

bool moveTo(int movement) {
    if (!isNewPositionAvailable(movement)) return false;
    deleteCurrentPiece();
    updateCoordinates(movement);
    drawNewPiece();
    return true;
}

bool isNewPositionAvailable(int movement) {
    // TODO: add game over
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int newx = calculateNewx(movement, piecePixelIndex);
        int newy = calculateNewy(movement, piecePixelIndex);
        if (isACurrentPiecePixel(newx, newy)) continue;
        if (!isAnActualAndTurnedOffPixel(newx, newy)) return false;
    }
    return true;
}

int calculateNewx(int movement, int piecePixelIndex) {
    int newx = x + pieceBaseCoordinates[pieceId][piecePixelIndex][XCOORDINATE];
    if (movement == LEFT) --newx;
    if (movement == RIGHT) ++newx;
    return newx;
}

int calculateNewy(int movement, int piecePixelIndex) {
    int newy = y + pieceBaseCoordinates[pieceId][piecePixelIndex][YCOORDINATE];
    if (movement == DOWN) ++newy;
    return newy;
}

bool isACurrentPiecePixel(int newx, int newy) {
    /*  When this function is called ledColorMatrix is not updated yet.
     *  isAnActualAndTurnedOffPixel would return true even if the new piece
     *  collides with some old coordinates of the still not updated ledColorMatrix
     */
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx = x + pieceBaseCoordinates[pieceId][piecePixelIndex][XCOORDINATE];
        int positiony = y + pieceBaseCoordinates[pieceId][piecePixelIndex][YCOORDINATE];
        if (newx == positionx && newy == positiony) return true;
    }
    return false;
}

void deleteCurrentPiece() {
    for (int piecePixelIndex = 0; piecePixelIndex < 4; ++piecePixelIndex) {
        int positionx = x + pieceBaseCoordinates[pieceId][piecePixelIndex][XCOORDINATE];
        int positiony = y + pieceBaseCoordinates[pieceId][piecePixelIndex][YCOORDINATE];
        deletePixel(positionx, positiony);
    }
}

void updateCoordinates(int movement) {
    if (movement == LEFT) --x;
    if (movement == RIGHT) ++x;
    if (movement == DOWN) ++y;
}

void fall() {
    if (!isNewPositionAvailable(DOWN)) {
        isNewPieceSpawnable = true;
        return;
    }
    moveTo(DOWN);
}
