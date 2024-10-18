#include <Arduino.h>
#include "BLE_manager.h"
#include "WiFi_manager.h"
#include "RTC_manager.h"
#include "Battery_monitor.h"
#include "EEPROM_manager.h"
#include "Command_handler.h"
#include "DeviceConfig.h"
#include "LTE_manager.h"
#include "Transmission_manager.h"

unsigned long previousMillis = 0;
const long interval = 30000;  // min 10 sec
const long wifiScanInterval = interval - 6000;
const long servingcellInterval = interval - 2000;
const long neighbourcellInterval = interval - 1000;

bool isWifiScanSent = false;
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

    // BLE 연결 상태 처리
    handleBLEConnectionChanges();

    // BLE 연결 여부와 상관없이 WiFi 스캔 수행
    if (currentMillis - previousMillis >= wifiScanInterval && !isWifiScanSent) {
        ScanAndSend();  // WiFi 스캔
        isWifiScanSent = true;
    }

    // LTE 데이터 수집 및 전송 처리
    if (currentMillis - previousMillis >= servingcellInterval && !isServingcellSent) {
        LTE_manager_sendATCommand("AT+QENG=\"servingcell\"\r\n");  // Serving cell 정보 수집
        isServingcellSent = true;
    }

    if (currentMillis - previousMillis >= neighbourcellInterval && !isNeighbourcellSent) {
        LTE_manager_sendATCommand("AT+QENG=\"neighbourcell\"\r\n");  // Neighbour cell 정보 수집
        isNeighbourcellSent = true;
    }

    // 주기적으로 서버로 데이터 전송 (10초 주기)
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // 타이머 리셋
        
        isWifiScanSent = false;
        isServingcellSent = false;
        isNeighbourcellSent = false;

        // 주기적으로 TCP 연결하여 데이터 전송
        transmitData();  // 데이터를 패킷화하여 서버로 전송
    }

    // 시리얼 버퍼 읽기 및 처리
    LTE_manager_readSerialBuffer();
}
