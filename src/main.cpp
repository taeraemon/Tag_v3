#include <Arduino.h>
#include "BLE_manager.h"
#include "WiFi_manager.h"
#include "RTC_manager.h"
#include "Battery_monitor.h"
#include "EEPROM_manager.h"
#include "Command_handler.h"
#include "DeviceConfig.h"
#include "LTEmanager.h"

unsigned long previousMillis = 0;
const long interval = 10000;  // 10 seconds for server transmission
const long servingcellInterval = 8000;  // 8 seconds for "servingcell"
const long neighbourcellInterval = 9000;  // 9 seconds for "neighbourcell"

bool isServingcellSent = false;
bool isNeighbourcellSent = false;

void setup() {
    Serial.begin(115200);
    Serial2.begin(115200);

    // EEPROM 초기화
    initEEPROM();
    initRTC();
    printTime();

    // 배터리 상태 모니터링 초기화
    initBatteryMonitor();

    // DeviceConfig
    DeviceConfig& config = DeviceConfig::getInstance();
    loadDeviceConfigFromEEPROM();

    // BLE 및 WiFi 초기화
    StartBLE();
    StartWiFi();

    Serial.println("Setup complete");
}

void loop() {
    unsigned long currentMillis = millis();

    // BLE 연결이 활성화된 경우 데이터 스캔 및 전송 처리
    if (isBLEConnected()) {
        if (isScanEnabled()) {
            ScanAndSend();
            delay(100); // 스캔 후 딜레이
        }
    }

    // BLE 연결 상태 처리
    handleBLEConnectionChanges();

    // LTE 데이터 수집 및 전송 처리
    if (currentMillis - previousMillis >= servingcellInterval && !isServingcellSent) {
        LTEmanager_sendATCommand("AT+QENG=\"servingcell\"\r\n");  // Serving cell 정보 수집
        isServingcellSent = true;
    }

    if (currentMillis - previousMillis >= neighbourcellInterval && !isNeighbourcellSent) {
        LTEmanager_sendATCommand("AT+QENG=\"neighbourcell\"\r\n");  // Neighbour cell 정보 수집
        isNeighbourcellSent = true;
    }

    // 주기적으로 서버로 데이터 전송 (10초 주기)
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // 타이머 리셋
        isServingcellSent = false;       // 다음 주기를 위해 리셋
        isNeighbourcellSent = false;

        // connectTCP();  // TCP 연결 후 데이터 전송
        Serial.println("Try to Send");
    }

    // 시리얼 버퍼 읽기 및 처리
    LTEmanager_readSerialBuffer();
}
