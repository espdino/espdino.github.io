#pragma once

const byte height = 16, width = 22, pW = 8, pH = 11;
byte birdCnt = 0, temp;
byte birdPos[2];
bool birdState[2];
bool gameData[height][width];
bool rowDirection = 1, movementCheck, jumpAscending = 1, isDucking = 0, wasDucking = 0, checkFlag = 0;

int scoreNumbers[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

int pixelsToClear[4][12]=
{
  131,132,133,124,123,122,99,100,92,91,1520,0,
  102,103,90,89,88,70,71,1520,0,0,0,0,
  30,29,33,58,1520,0,0,0,0,0,0,0,
  27,26,36,61,1520,0,0,0,0,0,0,0 
};

unsigned int upButtonState = 0, downButtonState = 0;
bool doingStuff = 0;

int ledIndex = 0, ledPosA,ledPosB;


unsigned long millisToMicros = 1;
#define timeFunc() ((millisToMicros > 999) ? micros() : millis())

int jumpPhase = 0, walkTime = 140 * millisToMicros, minGen = 1500 * millisToMicros, maxGen = 2200 * millisToMicros, score = 1, i = 0, j = 0, k = 0, brightness = 25;

int speedIncrease = 1 * millisToMicros;
int minSpeed = 59 * millisToMicros;
int startSpeed = 69 * millisToMicros;

unsigned long moveTimer, walkTimer, obstacleGenTimer, jumpTimer = 0, buttonsTimer = 0,  // timers for each operation
  bird1 = 0, bird2 = 0, moveTime = startSpeed, currMils = 0,
                                                      flapTime = 200 * millisToMicros, duckTime = 80 * millisToMicros, checkButtons = 30 * millisToMicros,
                                                      genTime = 200 * millisToMicros, timeToSpeedUp = 100;

unsigned long jumpTime[] = { 5 * millisToMicros, 40 * millisToMicros, 75 * millisToMicros, 140 * millisToMicros, 350 * millisToMicros };

byte obType = 1, currState = 1, previousState = 1;

bool player[pH][pW];
