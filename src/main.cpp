#include <Arduino.h>
#include "BLE_manager.h"
#include "WiFi_manager.h"
#include "RTC_manager.h"
#include "Battery_monitor.h"
#include "EEPROM_manager.h"
#include "Command_handler.h"
#include "DeviceConfig.h"

void setup() {
    Serial.begin(115200);

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
    // BLE 연결이 활성화된 경우, 스캔 및 데이터 전송 처리
    if (isBLEConnected()) {
        if (isScanEnabled()) {
            ScanAndSend();
            delay(100); // 스캔 후 딜레이
        }
    }

    // BLE 연결 상태 처리
    handleBLEConnectionChanges();
}
