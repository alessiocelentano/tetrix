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

#define CLK            8
#define LAT            10
#define OE             9
#define A              A0
#define B              A1
#define C              A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
int pieces[7][4][2] = { 
    IPIECE, SQUAREPIECE, LPIECE,
    LREVERSEDPIECE, ZPIECE,
    ZREVERSEDPIECE, TPIECE
};
unsigned int colors[7] = {
    LIGHTBLUE, YELLOW, ORANGE,
    BLUE, RED, GREEN, PURPLE
};
bool ledStateMatrix[16][32] = {0};
bool isNewPieceSpawnable = true;
unsigned long previousTime = 0;
unsigned long currentTime = 0;
int pieceId;
int x, y;

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
    while (currentTime - previousTime < DELAY) {
        currentTime = millis();
        getInput();
        delay(INPUTDELAY);
    }
    previousTime = currentTime;
    fall();
}

void deleteFullLines() {
    for (int positiony = DOWNLIMIT; positiony > UPLIMIT; --positiony) {
        if (!isLineFull(positiony)) continue;
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
            deletePixel(positionx, positiony);
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
    if (ledStateMatrix[positionx][positiony]) return false;
    return true;
}

void deletePixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, NOCOLOR);
    ledStateMatrix[positionx][positiony] = 0;
}

void dropLinesFrom(int positiony) {
    while (positiony-- > 0) {
        for (int positionx = LEFTLIMIT; positionx <= RIGHTLIMIT; ++positionx)
            dropPixelByOnePosition(positionx, positiony);
    }
}

void dropPixelByOnePosition(int positionx, int positiony) {
    // TODO: store color of pixels
    deletePixel(positionx, positiony++);
    drawPixel(positionx, positiony);
}

void drawPixel(int positionx, int positiony) {
    matrix.drawPixel(positiony, positionx, colors[pieceId]);
    ledStateMatrix[positionx][positiony] = 1;
}

void createNewPiece() {
    x = SPAWNX;
    y = SPAWNY;
    pieceId = rand() % 7;
    drawNewPiece();
}

void drawNewPiece() {
    for (int i = 0; i < 4; ++i) {
        int positionx = x + pieces[pieceId][i][0];
        int positiony = y + pieces[pieceId][i][1];
        drawPixel(positionx, positiony);
    }
}

void getInput() {
    // if (digitalRead(UPPIN)) hardDrop();
    if (digitalRead(LEFTPIN)) moveTo(LEFT);
    if (digitalRead(RIGHTPIN)) moveTo(RIGHT);
    if (digitalRead(DOWNPIN)) moveTo(DOWN);
}

void hardDrop() {
    while (moveTo(DOWN));
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
    turnPieceLedStates(OFF);
    for (int i = 0; i < 4; ++i) {
        int newx = calculateNewx(movement, i);
        int newy = calculateNewy(movement, i);
        if (!isAnActualAndTurnedOffPixel(newx, newy)) {
            turnPieceLedStates(ON);
            return false;
        }
    }
    turnPieceLedStates(ON);
    return true;
}

void turnPieceLedStates(int state) {
    /*  turn off piece leds on ledStateMatrix temporarily
     *  in order to not collide with old piece coordinates 
     */
    for (int i = 0; i < 4; ++i) {
        int positionx = x + pieces[pieceId][i][0];
        int positiony = y + pieces[pieceId][i][1];
        ledStateMatrix[positionx][positiony] = state;
    }
}

int calculateNewx(int movement, int index) {
    int newx = x + pieces[pieceId][index][0];
    if (movement == LEFT) --newx;
    if (movement == RIGHT) ++newx;
    return newx;
}

int calculateNewy(int movement, int index) {
    int newy = y + pieces[pieceId][index][1];
    if (movement == DOWN) ++newy;
    return newy;
}

void deleteCurrentPiece() {
    for (int i = 0; i < 4; ++i) {
        int positionx = x + pieces[pieceId][i][0];
        int positiony = y + pieces[pieceId][i][1];
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

/*
void startOst() {
        int val = 500;
        for (int i = 0; i < 12; ++i) {
            tone(PIEZO, val);
            val += 25;
            delay(500);
        }
}
*/
