#pragma once

#include <Adafruit_NeoPixel.h>
#include "gameVariables.h"
#include "sprites.h"
#define LED_PIN 13
#define LED_COUNT 256

const uint8_t* digitBitmaps[] = { zero, one, two, three, four, five, six, seven, eight, nine };

//Инициализация на NeoPixel матрицата.
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Задава три основни цвята – за кактуси (зелен), играч (червен), и птици (оранжев).
//gamma32() коригира цветовете за по-реалистична яркост на NeoPixel-ите.
uint32_t cactus_color = strip.gamma32(strip.Color(30, 255, 100));
uint32_t player_color = strip.gamma32(strip.Color(255, 30, 30));
uint32_t birds_color = strip.gamma32(strip.Color(255, 130, 0));

//Изчиства матрицата и я запълва според данните в двумерния масив gameData.
void fillStrip(byte r, byte g, byte b) {
  rowDirection = 0;
  strip.clear();
  for (int i = 0; i < 16; i++) {
    for (int j = 0; j < 16; j++) {
      ledIndex = (i + 1) * 16 - j - 1;
      if (gameData[i][j] && rowDirection)
        strip.setPixelColor(ledIndex, r, g, b);
      else if (gameData[i][j])
        strip.setPixelColor((i * 16 + j), r, g, b);
    }  // set colors
    rowDirection = !rowDirection;
  }
}

//Добавя зелена лента трева на долния ред (първите 16 пиксела).
void grass() {
  for (int i = 0; i < 16; i++) {
    strip.setPixelColor(i, 0, 255, 0);
  }
}

//Нулира игровите масиви gameData и player
void resetGameArrays() {
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) { gameData[i][j] = 0; }
  }
  for (int i = 0; i < pW; i++) {
    for (int j = 0; j < pH; j++) { player[j][i] = 0; }
  }
}

//Изпълнява анимация при край на играта: мига червено няколко пъти.
//След това показва надпис "YOUR SCORE IS:" и после числовия резултат.
//Използва масиви scoreText и digitBitmaps (със символи).
//Изчаква да бъде натиснат бутон (up или down), преди да рестартира.
//Нулира всички игрови променливи: състояние, фази, таймери и позициите на врагове.
//Рисува отново тревата и рестартира играта.
void gameOver(int Score) {
  /*Serial.println(moveTimer);
  Serial.println(walkTimer);
  Serial.println(obstacleGenTimer);
  Serial.println(jumpTimer);
  Serial.println(buttonsTimer);*/
  int flashDelay = 175;
  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, 255, 0, 0);
  strip.show();
  delay(flashDelay);
  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delay(flashDelay);
  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, 255, 0, 0);
  strip.show();
  delay(flashDelay);
  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, 0, 0, 0);
  strip.show();
  delay(flashDelay);
  for (int i = 0; i < 256; i++)
    strip.setPixelColor(i, 255, 0, 0);
  strip.show();
  delay(flashDelay);
  int delayT = 30;
  for (i = 0; i < height; i++) {
    for (j = 0; j < width; j++) {
      gameData[i][j] = 0;
    }
  }
  for (int i = 0; i < 6; i++) {
    for (int k = 0; k < 8; k++) {
      for (int j = 0; j < 16; j++) {
        gameData[15 - j][15] = bitRead(scoreText[j * 6 + i], 7 - k);
      }
      fillStrip(255, 0, 0);
      strip.show();
      delay(delayT);
      for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
          gameData[j][i] = gameData[j][i + 1];
        }
      }
    }
  }

  int len = 0;
  while (Score) {
    scoreNumbers[len] = Score % 10;
    Score /= 10;
    len++;
  }
  for (int N = len - 1; N > -1; N--) {
    for (int i = 0; i < 6; i++) {
      for (int j = 0; j < 8; j++) {

        gameData[12 - j][15] = bitRead(pgm_read_byte(&digitBitmaps[scoreNumbers[N]][i]), 7 - j);
      }
      fillStrip(255, 0, 0);
      strip.show();
      delay(delayT + 30);
      for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
          gameData[i][j] = gameData[i][j + 1];
        }
      }
    }
  }
  upButtonState = downButtonState = 0;
  while (!upButtonState && !downButtonState) {
    delay(10);
  }
  delay(500);
  resetGameArrays();
  strip.clear();
  score = 1;
  currState = 1;
  upButtonState = 0;
  downButtonState = 0;
  isDucking = 0;
  wasDucking = 0;
  birdCnt = 0;
  jumpPhase = 0;
  checkFlag = 0;
  jumpAscending = 1;
  birdPos[0] = birdPos[1] = 16;
  moveTime = startSpeed;
  currMils = timeFunc();
  moveTimer = timeFunc();
  walkTimer = timeFunc();
  obstacleGenTimer = timeFunc();
  strip.clear();
  grass();
  strip.show();
}

//Проверява за сблъсък между играча (player[][]) и обектите в gameData[][].
bool checkForCollision() {
  for (int i = 0; i < 9; i++) {
    for (int j = 0; j < 8; j++) {
      if (player[i][j] && gameData[i + 1][j]) {
        return 1;
      }
    }
  }
  return 0;
}

//Управлява движението и визуализацията на динозавъра:
//cState – текущо състояние (ходене, приклякане, скок).
//Поддържа променливи jumpPhase, isDucking, wasDucking, jumpAscending.
//Разделя се на три основни режима:
//Скачане (cState == 3): обновява позицията нагоре/надолу по фази.
//Приклякане (isDucking == true): сменя спрайта на динозавъра.
//Нормално ходене: редува два кадъра за ходене.
//Използва предварително дефинирани 8×8 пикселни спрайтове от dino[][][].
//В края визуализира player върху LED матрицата.
void playerMovement(byte cState) {
  if (cState == 3) {
    movementCheck = (currMils - jumpTimer > jumpTime[jumpPhase]);

    if (movementCheck && !jumpAscending)  // down part of the jump
    {
      for (int i = 0; i < 6; i++) {
        player[jumpPhase + 7][i] = 0;
      }

      for (int i = jumpPhase; i < jumpPhase + 8; i++) {
        for (int j = 0; j < 6; j++) {
          strip.setPixelColor(playerLedPos[i][j], 0, 0, 0);
        }
      }

      jumpTimer = timeFunc();
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          player[jumpPhase - 1 + i][j] = dino[4][7 - i][j];
        }
      }
      jumpPhase--;
      if (jumpPhase == 0) {
        jumpAscending = 1;
        currState = 1;
        jumpPhase = 0;
      }
    }

    else if (movementCheck && jumpAscending)  // up part of the jump
    {
      for (int i = 0; i < 6; i++) {
        player[jumpPhase][i] = 0;
      }
      for (int i = jumpPhase; i < jumpPhase + 8; i++) {
        for (int j = 0; j < 6; j++) {
          strip.setPixelColor(playerLedPos[i][j], 0, 0, 0);
        }
      }
      jumpTimer = timeFunc();
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 6; j++) {
          player[jumpPhase + 1 + i][j] = dino[4][7 - i][j];
        }
      }
      jumpPhase++;
      if (jumpPhase == 4) {
        jumpAscending = 0;
      }
    }
  }

  else if (isDucking) {

    if (!wasDucking) {
      wasDucking = 1;
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[0][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[0][i], 0, 0, 0);
      }
    }

    if (cState == 1) {
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[2][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[2][i], 0, 0, 0);
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          player[i][j] = dino[2][7 - i][j];
        }
      }
    }

    else if (cState == 2) {
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[3][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[3][i], 0, 0, 0);
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          player[i][j] = dino[3][7 - i][j];
        }
      }
    }
  } else {
    if (wasDucking) {
      wasDucking = 0;
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[1][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[1][i], 0, 0, 0);
      }
    }

    if (cState == 1) {
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[2][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[2][i], 0, 0, 0);
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          player[i][j] = dino[0][7 - i][j];
        }
      }
    }

    else if (cState == 2) {
      for (int i = 0; i < 12; i++) {
        if (pixelsToClear[3][i] == 1520)
          break;
        strip.setPixelColor(pixelsToClear[3][i], 0, 0, 0);
      }
      for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
          player[i][j] = dino[1][7 - i][j];
        }
      }
    }
  }

  rowDirection = 0;
  for (int i = 0; i < 11; i++) {
    for (int j = 0; j < 8; j++) {
      ledPosA = i * 16 + j + 16;
      ledPosB = i * 16 - j + 31;
      if (player[i][j] && rowDirection)
        strip.setPixelColor(ledPosA, player_color);
      else if (player[i][j])
        strip.setPixelColor(ledPosB, player_color);
    }
    rowDirection = !rowDirection;
  }
}

//Генерира ново препятствие вдясно на екрана.
//objType определя вида:
//1–3 → различни видове кактуси (cacti1, cacti2, cacti3).
//4 → птица (летящ враг), която се анимира по-късно.
//Задава начални координати и записва пикселите в gameData.
void generateObstacle(byte objType) {
  if (objType == 1) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        gameData[1 + i][15 + j] = cacti1[2 - i][j];
      }
    }
  } else if (objType == 2) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        gameData[1 + i][15 + j] = cacti2[2 - i][j];
      }
    }
  } else if (objType == 3) {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++) {
        gameData[1 + i][15 + j] = cacti3[2 - i][j];
      }
    }
  }

  else {
    birdCnt++;
    birdState[birdCnt - 1] = 1;
    birdPos[birdCnt - 1] = 16;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        gameData[7 + i][16 + j] = birdO[2 - i][j];
      }
    }
  }
}

//Проверява състоянието на бутоните (upButtonState, downButtonState) през определен интервал (checkButtons).
void updateButtonsState() {
  if ((currMils - buttonsTimer) > checkButtons) {
    buttonsTimer = timeFunc();
    if (upButtonState && !downButtonState) {
      currState = 3;
      isDucking = 0;
    } else if (!upButtonState && downButtonState) {
      isDucking = 1;
    } else
      isDucking = 0;
  }
}

//Режим за настройка на яркостта.
//Докато downButtonState != 200, позволява промяна на яркостта чрез up бутона.
//Обновява дисплея с текущия персонаж и тревата за визуален ориентир.
void adjustBrightness() {
  while (downButtonState != 200) {
    if (upButtonState > 0) brightness = upButtonState;
    strip.setBrightness(brightness);
    playerMovement(1);
    grass();
    strip.show();
  }
}

//Определя следващия обект за генериране (кактус или птица).
//Използва random() за разнообразие и настройва таймери за следващото препятствие.
//Следи birdCnt, за да не се появяват твърде много птици наведнъж.
void generateObstacles() {
  if (birdCnt == 2 && obType == 4)
    generateObstacle(2);
  else
    generateObstacle(obType);
  randomSeed(analogRead(A3));
  obstacleGenTimer = timeFunc();
  genTime = random(minGen, maxGen) * millisToMicros;
  obType = random(1, 5);
  if (birdCnt && obType != 4) {
    checkFlag = 1;
    genTime = random(minGen, maxGen) * millisToMicros;
  }
}

//Анимира летящите птици чрез редуване между две позиции на крилата (birdT и birdO).
//Обновява gameData с новата форма.
//Ако има две птици, управлява ги поотделно с отделни таймери (bird1, bird2).
void birdsMovement() {
  bird1 = timeFunc();
  if (birdState[0]) {
    birdState[0] = 0;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        gameData[7 + i][birdPos[0] + j] = birdT[2 - i][j];
      }
    }
  } else {
    birdState[0] = 1;
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 5; j++) {
        gameData[7 + i][birdPos[0] + j] = birdO[2 - i][j];
      }
    }
  }
  if (birdCnt == 2) {
    if (currMils - bird2 > flapTime) {
      bird2 = timeFunc();
      if (birdState[1]) {
        birdState[1] = 0;
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 5; j++) {
            gameData[7 + i][birdPos[1] + j] = birdT[2 - i][j];
          }
        }
      } else {
        birdState[1] = 1;
        for (int i = 0; i < 3; i++) {
          for (int j = 0; j < 5; j++) {
            gameData[7 + i][birdPos[1] + j] = birdO[2 - i][j];
          }
        }
      }
    }
  }
}

void setCactiColor() {
  rowDirection = 0;
  strip.clear();
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 16; j++) {
      ledIndex = (i + 1) * 16 - j - 1;
      if (gameData[i][j] && rowDirection)
        strip.setPixelColor(ledIndex, cactus_color);
      else if (gameData[i][j])
        strip.setPixelColor((i * 16 + j), cactus_color);
    }  // set colors
    rowDirection = !rowDirection;
  }
}
//Присвояват правилните цветове (зелено за кактуси, оранжево за птици).
//Рисуват само съответните редове от матрицата.
void setBirdsColor() {
  rowDirection = 0;
  for (int i = 4; i < 12; i++) {
    for (int j = 0; j < 16; j++) {
      ledIndex = (i + 1) * 16 - j - 1;
      if (gameData[i][j] && rowDirection)
        strip.setPixelColor(ledIndex, birds_color);
      else if (gameData[i][j])
        strip.setPixelColor((i * 16 + j), birds_color);
    }  // set colors
    rowDirection = !rowDirection;
  }
}

//Премества всички обекти наляво (симулира движение).
//Ако има птици — обновява и техните позиции.
//Изчиства крайните колони, за да не остават следи.
//Проверява за сблъсък:
//Ако няма → увеличава резултата, рисува нови обекти, ускорява играта.
//Ако има → извиква gameOver(score).
void moveObstacles() {
  for (int i = 0; i < width - 1; i++) {
    for (int j = 0; j < height; j++) {
      gameData[j][i] = gameData[j][i + 1];
    }
  }
  if (birdCnt) {
    for (int i = 0; i < birdCnt; i++) {
      birdPos[i]--;
    }
    if (birdPos[0] < 1 && birdCnt == 2) {

      birdPos[0] = birdPos[1];
      birdCnt--;
    } else if (birdPos[0] < 1) {
      birdCnt--;
    }
  }
  for (int i = 7; i < 11; i++)
    gameData[i][width - 1] = 0;
  for (int i = 1; i < 4; i++)
    gameData[i][width - 1] = 0;
  if (!checkForCollision()) {
    score++;
    setCactiColor();
    setBirdsColor();
    moveTimer = timeFunc();
  } else
    gameOver(score);
  if (score % timeToSpeedUp == 0) {

    moveTime -= speedIncrease;
    if (moveTime < minSpeed)
      moveTime = minSpeed;
  }

}
