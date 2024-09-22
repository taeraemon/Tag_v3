#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <esp_bt.h>
#include <esp_gap_ble_api.h>

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

void StartBLE();
void handleBLEConnectionChanges();
bool isBLEConnected();
void updateBLEDeviceName(const char* name);
void setBLEAdvertisingInterval(int interval);  // BLE advertise interval 설정 함수
void adjustBLETransmitPower(int level);  // 0~9 값을 받아 BLE 송신 전력 설정 함수
void sendNotification(const char* message);  // 기본 BLE 알림 전송 함수
void notifyBatteryStatus(int batteryLevel);  // 배터리 상태 전송
void notifyWiFiStatus(const char* ssid, int rssi);  // WiFi 상태 전송

class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer);
    void onDisconnect(BLEServer* pServer);
};

class MyCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic);  // 명령 수신 처리
};

#endif // BLE_MANAGER_H
