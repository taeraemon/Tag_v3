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
        char cmd[200] = {0};
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
            case CMD_SERVER:
                handleServerIPCommand(cmd);
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

    // 임시로 SSID를 복사
    char tempSSID[100];  // SSID 최대 길이를 고려하여 충분히 큰 버퍼 사용
    strcpy(tempSSID, &cmd[2]);  // cmd[2] 이후의 문자열을 복사

    // \r 또는 \n 제거
    removeTrailingNewlines(tempSSID);

    // SSID 설정
    config.setSSID(tempSSID);  // 제거된 SSID 설정

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


// Scan or Advertise Config (Interval, Power)
void handleConfigCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();
    int val = atoi(&cmd[2]);

    // Scan Interval 설정
    if (cmd[1] == '1') {
        config.setScanInterval(val);  // Advertise 주기 설정
        
        // EEPROM에 저장
        char buffer[10];
        itoa(val, buffer, 10);
        writeEEPROM(EEPROM_ADDR_INTV, buffer);
    }
    
    // 송신 전력 설정
    else if (cmd[1] == '2') {
        config.setTransmitPower(val);  // 송신 전력 설정

        // WiFi 및 BLE 송신 전력 설정
        adjustWiFiTransmitPower(val);
        adjustBLETransmitPower(val);
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

// Server IP 변경
void handleServerIPCommand(const char* cmd) {
    // 임시로 서버 IP를 복사
    char tempIP[150];  // 서버 IP 최대 길이를 고려하여 충분히 큰 버퍼 사용
    strcpy(tempIP, &cmd[1]);  // cmd[1] 이후의 문자열을 복사

    // \r 또는 \n 제거
    removeTrailingNewlines(tempIP);

    DeviceConfig& config = DeviceConfig::getInstance();

    // 서버 IP 설정
    config.setServerIP(tempIP);  // 불필요한 문자가 제거된 서버 IP 설정

    // EEPROM에 저장
    writeEEPROM(EEPROM_ADDR_SEVR, config.getServerIP());

    // 업데이트된 서버 IP 출력
    Serial.print("Server IP Modified: ");
    Serial.println(config.getServerIP());
}

// 문자열 끝에서 \r, \n 또는 \r\n 제거하는 함수
void removeTrailingNewlines(char* str) {
    int length = strlen(str);

    // 문자열이 비어있지 않다면 검사
    while (length > 0 && (str[length - 1] == '\r' || str[length - 1] == '\n')) {
        str[length - 1] = '\0';  // 널 문자로 대체하여 제거
        length--;  // 다음 문자를 확인
    }
}
