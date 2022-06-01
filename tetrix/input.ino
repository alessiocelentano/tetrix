void getInput(int delaytime) {
    unsigned long long previousTime = millis();
    while (millis() - previousTime < delaytime) {
        controller.update();
        updatePosition();
        delay(INPUTDELAY2);
    }
}

void updatePosition() {
    checkForInput(LEFT);
    checkForInput(RIGHT);
    checkForInput(DOWN);
    checkForInput(HARDDROP);
    checkForInput(ROTATE);
}

void checkForInput(int button) {
    if (isPressed(button)) {
        doAction(button);
        lockButton(button);
    } else {
        unlockButton(button);
    }
}

bool isPressed(int button) {
    if (controller.analogX < 80 && button == LEFT) return true;
    if (controller.analogX > 180 && button == RIGHT) return true;
    if (controller.analogY < 80 && button == DOWN) return true;
    if (controller.cButton && button == HARDDROP)  return true;
    if (controller.zButton && button == ROTATE) return true;
    return false;
}

void doAction(int button) {
    if (button == LEFT || button == RIGHT || button == DOWN) doAdaptiveMovementTo(button);
    if (!cButtonLock && button == HARDDROP) hardDrop(x, y);
    if (!zButtonLock && button == ROTATE) rotate();
}

void doAdaptiveMovementTo(int button) {
    if (isLocked(button)) {
        moveTo(button);
        movementTime = millis();
    } else if (millis() - movementTime > MOVEMENTDELAY) {
        moveTo(button);
        delay(INPUTDELAY);
    }
}

bool isLocked(int button) {
    if (!leftLock && button == LEFT) return true;
    if (!rightLock && button == RIGHT) return true;
    if (!downLock && button == DOWN) return true;
    return false;
}

void moveTo(int button) {
    if (button == LEFT) move(x+1, y);
    if (button == RIGHT) move(x-1, y);
    if (button == DOWN) move(x, y+1);
}

void lockButton(int button) {
    if (button == LEFT) leftLock = true;
    if (button == RIGHT) rightLock = true;
    if (button == DOWN) downLock = true;
    if (button == HARDDROP) cButtonLock = true;
    if (button == ROTATE) zButtonLock = true;
}

void unlockButton(int button) {
    if (button == LEFT) leftLock = false;
    if (button == RIGHT) rightLock = false;
    if (button == DOWN) downLock = false;
    if (button == HARDDROP) cButtonLock = false;
    if (button == ROTATE) zButtonLock = false;
}

bool move(int positionx, int positiony) {
    if (!isPositionAvailable(positionx, positiony, rotationState)) return false;
    deleteCurrentPiece();
    deleteShadow();
    x = positionx;
    y = positiony;
    createShadow();
    drawNewPiece(positionx, positiony, color);
    return true;
}

void hardDrop(int positionx, int positiony) {
    while (move(positionx, ++positiony)) delay(5);
    setupNewTurn();
}

void rotate() {
    if (pieceID == SQUARE_ID) return;
    if (!isPositionAvailable(x, y, rotationState+1)) return;
    deleteCurrentPiece();
    deleteShadow();
    ++rotationState;
    createShadow();
    drawNewPiece(x, y, color);
}
