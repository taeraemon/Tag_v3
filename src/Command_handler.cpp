#include <Arduino.h>
#include "Command_handler.h"
#include "EEPROM_manager.h"
#include "WiFi_manager.h"
#include "BLE_manager.h"
#include "DeviceConfig.h"
#include "RTC_manager.h"
#include "Battery_monitor.h"

void processCommand(const std::string &command) {
    Serial.print("Received Command: ");
    Serial.println(command.c_str());

    if (command.length() > 0) {
        char cmd[100] = {0};
        strcpy(cmd, command.c_str());

        switch (cmd[0]) {
            case CMD_SCAN:
                handleScanCommand(cmd);
                break;
            case CMD_SSID:
                handleSSIDCommand(cmd);
                break;
            case CMD_CONFIG:
                handleConfigCommand(cmd);
                break;
            case CMD_BATTERY:
                handleBatteryCommand();
                break;
            case CMD_TIME:
                handleTimeCommand(cmd);
                break;
            case CMD_RESET:
                handleResetCommand();
                break;
            default:
                Serial.println("Unknown command.");
                break;
        }
    }
}

// 스캔 기능 토글
void handleScanCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();

    if (cmd[1] == '0') {
        config.setScanToggle(0);  // DeviceConfig에서 스캔 토글 설정
        writeEEPROM(EEPROM_ADDR_SCAN, (char*)"0");
        Serial.println("WiFi scan disabled");
    }
    else if (cmd[1] == '1') {
        config.setScanToggle(1);
        writeEEPROM(EEPROM_ADDR_SCAN, (char*)"1");
        Serial.println("WiFi scan enabled");
    }
}

// SSID 변경
void handleSSIDCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();

    // SSID 설정
    config.setSSID(&cmd[2]);  // DeviceConfig에서 SSID 설정

    // RAM 또는 EEPROM에 저장 여부 확인 (cmd[1] 값에 따라 결정)
    if (cmd[1] == '1') {
        writeEEPROM(EEPROM_ADDR_SSID, config.getSSID());
    }

    // BLE advertise 이름 동작 중 변경 (BLE_manager의 함수 호출)
    updateBLEDeviceName(config.getSSID());

    // WiFi 시작 (SSID 변경 후 재시작 필요)
    StartWiFi();
    Serial.print("SSID Modified : ");
    Serial.println(config.getSSID());
}

// BLE Advertise
void handleConfigCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();
    int val = atoi(&cmd[2]);

    if (cmd[1] == '1') {
        config.setAdvInterval(val);  // Advertise 주기 설정
        
        Serial.print("Advertising interval set to: ");
        Serial.println(config.getAdvInterval());
    }
    else if (cmd[1] == '2') {
        config.setTransmitPower(val);  // 송신 전력 설정
        Serial.print("Transmit power set to: ");
        Serial.println(config.getTransmitPower());
    }
}

// Battery Voltage Monitor
void handleBatteryCommand() {
    Serial.println("Battery monitor requested.");
    
    // 배터리 상태 주기적 전송
    int batteryLevel = getAverageBatteryLevel();
    notifyBatteryStatus(batteryLevel);
}

// RTC Time Handling
void handleTimeCommand(const char* cmd) {
    int newTime = atoi(&cmd[1]);
    setTime(newTime);
    Serial.print("Setting new time to: ");
    Serial.println(newTime);
}

// Factory Reset
void handleResetCommand() {
    Serial.println("Device reset requested.");
    resetEEPROM();
}
