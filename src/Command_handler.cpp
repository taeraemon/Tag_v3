#include <Arduino.h>
#include "Command_handler.h"
#include "EEPROM_manager.h"
#include "WiFi_manager.h"
#include "DeviceConfig.h"

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

void handleSSIDCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();
    config.setSSID(&cmd[2]);  // DeviceConfig에서 SSID 설정

    writeEEPROM(EEPROM_ADDR_SSID, config.getSSID());
    Serial.print("SSID set to: ");
    Serial.println(config.getSSID());
    StartWiFi();
}

void handleConfigCommand(const char* cmd) {
    DeviceConfig& config = DeviceConfig::getInstance();
    int val = atoi(&cmd[2]);

    if (cmd[1] == '1') {
        config.setAdvInterval(val);  // 광고 주기 설정
        Serial.print("Advertising interval set to: ");
        Serial.println(config.getAdvInterval());
    }
    else if (cmd[1] == '2') {
        config.setTransmitPower(val);  // 송신 전력 설정
        Serial.print("Transmit power set to: ");
        Serial.println(config.getTransmitPower());
    }
}

void handleBatteryCommand() {
    Serial.println("Battery monitor requested.");
}

void handleTimeCommand(const char* cmd) {
    int newTime = atoi(&cmd[1]);
    Serial.print("Setting new time to: ");
    Serial.println(newTime);
}

void handleResetCommand() {
    Serial.println("Device reset requested.");
    resetEEPROM();
}
