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
#define DELAY          200
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
        createNewPiece();
        isNewPieceSpawnable = false;
    }
    while (currentTime - previousTime < DELAY) {
        currentTime = millis();
        getInput();
    }
    previousTime = currentTime;
    fall();
}

void createNewPiece() {
    x = SPAWNX;
    y = SPAWNY;
    pieceId = rand() % 7;
    drawNewPiece();
}

void drawNewPiece() {
    int currentx, currenty;
    for (int i = 0; i < 4; i++) {
        currentx = x + pieces[pieceId][i][0];
        currenty = y + pieces[pieceId][i][1];
        matrix.drawPixel(currenty, currentx, colors[pieceId]);
        ledStateMatrix[currentx][currenty] = 1;
    }
}

void getInput() {
    if (digitalRead(UP)) hardDrop();
    if (digitalRead(LEFT)) moveTo(LEFT);
    if (digitalRead(RIGHT)) moveTo(RIGHT);
    if (digitalRead(DOWN)) moveTo(DOWN);
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
    int newx, newy;
    turnPieceLedStates(OFF);
    for (int i = 0; i < 4; i++) {
        newx = x + pieces[pieceId][i][0];
        newy = y + pieces[pieceId][i][1];
        if (movement == LEFT) --newx;
        if (movement == RIGHT) ++newx;
        if (movement == DOWN) ++newy;
        if (!isPixelAvailable(newx, newy)) {
            turnPieceLedStates(ON);
            return false;
        }
    }
    turnPieceLedStates(ON);
    return true;
}

void turnPieceLedStates(int state) {
    int currentx, currenty;
    for (int i = 0; i < 4; i++) {
        currentx = x + pieces[pieceId][i][0];
        currenty = y + pieces[pieceId][i][1];
        ledStateMatrix[currentx][currenty] = state;
    }
}

bool isPixelAvailable(int newx, int newy) {
    if (ledStateMatrix[newx][newy]) return false;
    if (newx < LEFTLIMIT || newx > RIGHTLIMIT) return false;
    if (newy > DOWNLIMIT) return false;
    return true;
}

void updateCoordinates(int movement) {
    if (movement == LEFT) --x;
    if (movement == RIGHT) ++x;
    if (movement == DOWN) ++y;
}

void deleteCurrentPiece() {
    int currentx, currenty;
    for (int i = 0; i < 4; i++) {
        currentx = x + pieces[pieceId][i][0];
        currenty = y + pieces[pieceId][i][1];
        matrix.drawPixel(currenty, currentx, NOCOLOR);
        ledStateMatrix[currentx][currenty] = 0;
    }
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
        for (int i = 0; i < 12; i++) {
            tone(PIEZO, val);
            val += 25;
            delay(500);
        }
}
*/

void debug() {
    while (1) {
     matrix.fillRect(1, 0, 15, 15, matrix.Color333(7, 0, 0));
     delay(10000);
    }
}
