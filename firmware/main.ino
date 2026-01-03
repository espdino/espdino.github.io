#include "gameLogic.h"
#include "bluetoothLogic.h"

void setup() {
  // Инициализация на BLE устройството с име "Dino"
  BLEDevice::init("Dino");

  // Създаване на BLE сървър (устройство, което може да приема връзки от клиенти)
  pServer = BLEDevice::createServer();

  // Задаваме callback функции за събития като свързване и прекъсване на връзката
  pServer->setCallbacks(new MyServerCallbacks());

  // Създаваме BLE услуга с даден UUID (идентификатор)
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Създаваме BLE характеристика за бутона "Up" (нагоре)
// Тя има свойството "WRITE" — клиентът може да ѝ изпраща данни
  upButtonCharacteristic = pService->createCharacteristic(
    UPBUTTON_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);

  // Създаваме BLE характеристика за бутона "Down" (надолу)
  downButtonCharacteristic = pService->createCharacteristic(
    DOWNBUTTON_CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_WRITE);

  // Задаваме callback функции за събития при писане върху характеристиките
  // (например когато клиент изпрати команда)
  downButtonCharacteristic->setCallbacks(new MyCharacteristicCallbacks());
  upButtonCharacteristic->setCallbacks(new MyCharacteristicCallbacks());

  // Добавяме стандартен BLE descriptor (2902)
  upButtonCharacteristic->addDescriptor(new BLE2902());
  downButtonCharacteristic->addDescriptor(new BLE2902());

  // Стартираме услугата, за да стане достъпна за клиенти
  pService->start();

  // Подготвяме BLE рекламата (advertising) — това е сигналът, който устройството излъчва,
  // за да може клиентите да го открият
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  
  // Добавяме UUID на услугата, за да може клиентите да знаят какви услуги предлага устройството
  pAdvertising->addServiceUUID(SERVICE_UUID);
  
  // Определяме, че няма да има отделен "scan response" пакет
  pAdvertising->setScanResponse(false);
  
  // Настройка на минимален предпочитан параметър — 0x00 означава, че няма да се рекламира
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  
  // Стартираме рекламата — устройството вече е видимо и готово за връзка
  BLEDevice::startAdvertising();

  strip.begin();  // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();   // Turn OFF all pixels ASAP
  strip.setBrightness(255); 
  // Задаване на максимална яркост, за да се вижда динозавъра, при настройката на яркостта
  Serial.begin(115200);
  moveTimer = walkTimer = obstacleGenTimer = timeFunc(); // нулиране на таймерите
  strip.clear();

  //зануляване на работните масиви
  resetGameArrays();
  
  moveTimer = timeFunc();
  currMils = timeFunc();
  adjustBrightness();
  upButtonState = downButtonState = 0;// Инициализиране на състоянието на бутоните
  moveTimer = walkTimer = obstacleGenTimer = jumpTimer = buttonsTimer = timeFunc(); // нулиране на таймерите
}

void loop() {
  currMils = timeFunc(); //обновяване на текущо време
  updateButtonsState(); // обновяване на състоянието на бутоните

  if (currMils - moveTimer > moveTime) {
    moveObstacles();
  }

  if (currMils - walkTimer > walkTime) {
    if (currState == 1)
      currState = 2;
    else if (currState == 2)
      currState = 1;
    walkTimer = timeFunc();
  }

  if (currMils - obstacleGenTimer > genTime) {
    generateObstacles();
  }

  if (birdCnt) {
    if (currMils - bird1 > flapTime) {
      birdsMovement();
    }
  }
  playerMovement(currState);
  grass();
  strip.show();

}
