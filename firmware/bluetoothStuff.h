#pragma once

#include "gameLogic.h"
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// Създават се глобални указатели към BLE сървъра и характеристиките за двата бутона
BLEServer* pServer = NULL;
BLECharacteristic* upButtonCharacteristic = NULL;
BLECharacteristic* downButtonCharacteristic = NULL;

// Флагове за следене на състоянието на BLE връзката
bool deviceConnected = false;
bool oldDeviceConnected = false;

// Дефинират се уникални UUID стойности за BLE услугата и характеристиките
// Тези UUID се използват за идентификация при комуникация с клиента
#define SERVICE_UUID "19b10000-e8f2-537e-4f6c-d104768a1214"
#define DOWNBUTTON_CHARACTERISTIC_UUID "19b10001-e8f2-537e-4f6c-d104768a1214"
#define UPBUTTON_CHARACTERISTIC_UUID "19b10002-e8f2-537e-4f6c-d104768a1214"

// Клас за обратни извиквания (callbacks) при събития, свързани със сървъра
class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    // Извиква се, когато клиент се свърже към BLE сървъра
    deviceConnected = true;
    Serial.println("A device has connected");
  };

  void onDisconnect(BLEServer* pServer) {
    // Извиква се, когато клиент се разкачи
    deviceConnected = false;
    // Автоматично стартира повторно рекламата, за да може друг клиент да се свърже
    pServer->startAdvertising();
  }
};

// Клас за обратни извиквания при промени в характеристиките (напр. когато клиентът пише нови стойности)
class MyCharacteristicCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* pCharacteristic) {
    // Получава стойността, която клиентът е записал в характеристиката
    String value = pCharacteristic->getValue();

    if (value.length() > 0) {
      // Конвертира първия байт от получената стойност в int
      int receivedValue = static_cast<int>(value[0]);

      // Променлива, сигнализираща, че е получено ново действие
      doingStuff = 1;

      // Проверява в коя характеристика е записано — "Up" или "Down"
      if (pCharacteristic == upButtonCharacteristic) {
        Serial.print("Up event, written: ");
        Serial.println(receivedValue);  // Отпечатва получената стойност за дебъг
        upButtonState = receivedValue;  // Запазва новото състояние на бутона "Up"
      } else if (pCharacteristic == downButtonCharacteristic) {
        Serial.print("Down event, written: ");
        Serial.println(receivedValue);  // Отпечатва получената стойност за дебъг
        downButtonState = receivedValue;  // Запазва новото състояние на бутона "Down"
      }
    }
  }
};