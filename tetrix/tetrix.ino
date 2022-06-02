#include <Adafruit_GFX.h>
#include <RGBmatrixPanel.h>
#include <Wire.h>
#include <ArduinoNunchuk.h>

// Relative position from the center of rotation
#define I              { { -1, 0 }, { 0, 0 }, { 1, 0 }, { 2, 0 } }
#define SQUARE         { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } }
#define L              { { 1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define REVERSED_L     { { -1, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
#define Z              { { -1, 0 }, { 0, 0 }, { 0, 1 }, { 1, 1 } }
#define REVERSED_Z     { { 0, 0 }, { 1, 0 }, { -1, 1 }, { 0, 1 } }
#define T              { { 0, 0 }, { -1, 1 }, { 0, 1 }, { 1, 1 } }
 
#define NOCOLOR        matrix.Color888(0, 0, 0)
#define CYAN           matrix.Color888(0, 16, 16)
#define YELLOW         matrix.Color888(16, 16, 0)
#define ORANGE         matrix.Color888(48, 16, 0)
#define BLUE           matrix.Color888(0, 0, 16)
#define RED            matrix.Color888(16, 0, 0)
#define GREEN          matrix.Color888(0, 16, 0)
#define PURPLE         matrix.Color888(16, 0, 16)
#define WHITE          matrix.Color888(16, 16, 16)

#define NOPIECE         0
#define I_ID            1
#define SQUARE_ID       2
#define REVERSED_L_ID   3
#define L_ID            4
#define REVERSED_Z_ID   5
#define Z_ID            6
#define T_ID            7
 
#define SPAWNX          8
#define SPAWNY          0
#define SPAWNROTATION   0
#define UPLIMIT         0
#define LEFTLIMIT       0
#define RIGHTLIMIT      15
#define DOWNLIMIT       31
#define FALLDELAY       200

#define INPUTDELAY      1
#define MAXSPEEDDELAY   50
#define DELETIONDELAY   150
#define TRIGGERMAXSPEED 300
#define PLACEMENTDELAY  400
#define FRAMEDELAY      750
#define GAMEOVERDELAY   1500

#define LEFT            0
#define RIGHT           1
#define DOWN            2
#define HARDDROP        3
#define ROTATE          4
#define OFF             0
#define ON              1
#define XCOORDINATE     0
#define YCOORDINATE     1
 
#define CLK             8
#define OE              9
#define LAT             10
#define A               A0
#define B               A1
#define C               A2

RGBmatrixPanel matrix(A, B, C, CLK, LAT, OE, false);
ArduinoNunchuk controller = ArduinoNunchuk();
// 32 rows, each row has 16 LEDs
// Every LED has 8 possible values (colors) so it can be stored in 3 bit (from 000 to 111)
// We need 16 * 3 = 48 bit. An unsigned int contain 16 bits on Arduino, so we'll use 3 unsigned int
unsigned int ledColorMatrix[32][3];
unsigned long long movementTime;
unsigned int color;
char pieceID;
char x, y;
char shadowx;
char shadowy;
char rotationState;  // 4 states, each state is a 90 degrees counterclockwise rotation
// Locks are used to prevent unintentional spam
bool downLock;
bool leftLock;
bool rightLock;
bool cButtonLock;
bool zButtonLock;
char basePieceCoordinates[7][4][2] = {
    I, SQUARE, L,
    REVERSED_L, Z,
    REVERSED_Z, T
};

void setup() {
    matrix.begin();
    controller.init();
    randomSeed(analogRead(12));
    setupNewGame();
}

void loop() {
    getInput(FALLDELAY);
    if (canFall()) move(x, y+1);
    else setupNewTurn();
}
