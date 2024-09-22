#include <Arduino.h>
#include <EEPROM.h>
#include "EEPROM_manager.h"
#include "DeviceConfig.h"

// EEPROM 초기화 함수
bool initEEPROM() {
    if (!EEPROM.begin(EEPROM_SIZE)) {
        return false;   // EEPROM 초기화 실패
    }

    return true;    // EEPROM 초기화 성공
}

// EEPROM에 데이터 쓰기
void writeEEPROM(int addr, const char* data) {
    int length = strlen(data);
    
    EEPROM.write(addr, length);  // 첫 번째 바이트에 길이 저장
    for (int i = 0; i < length; i++) {
        EEPROM.write(addr + 1 + i, data[i]);  // 그 뒤로 데이터 저장
    }
    EEPROM.commit();  // 실제 EEPROM에 저장
}

// EEPROM에서 데이터 읽기 (첫 번째 바이트는 길이로 저장됨)
void readEEPROM(int addr, char* data) {
    int storedLength = EEPROM.read(addr);  // 첫 번째 바이트에서 길이 읽기
    for (int i = 0; i < storedLength; i++) {
        data[i] = EEPROM.read(addr + 1 + i);  // 데이터 읽기
    }
    data[storedLength] = '\0';  // 문자열 끝을 알리는 null 추가
}

// EEPROM 리셋 함수
void resetEEPROM() {
    // 기본 SSID와 비밀번호 설정
    writeEEPROM(EEPROM_ADDR_SSID, EEPROM_DFLT_SSID);
    writeEEPROM(EEPROM_ADDR_PSWD, EEPROM_DFLT_PSWD);

    // WiFi 스캔 설정
    writeEEPROM(EEPROM_ADDR_SCAN, EEPROM_DFLT_SCAN);

    // Advertise 주기 설정
    writeEEPROM(EEPROM_ADDR_INTV, EEPROM_DFLT_INTV);

    // 최대 송신 전력 설정
    writeEEPROM(EEPROM_ADDR_TXPW, EEPROM_DFLT_TXPW);
}

// EEPROM에서 DeviceConfig 값 로드
void loadDeviceConfigFromEEPROM() {
    DeviceConfig& config = DeviceConfig::getInstance();

    // SSID 읽기
    char ssid[100];
    readEEPROM(EEPROM_ADDR_SSID, ssid);
    config.setSSID(ssid);

    // 비밀번호 읽기
    char pswd[100];
    readEEPROM(EEPROM_ADDR_PSWD, pswd);
    config.setPassword(pswd);

    // WiFi 스캔 설정 읽기
    char scan_toggle[10];
    readEEPROM(EEPROM_ADDR_SCAN, scan_toggle);
    config.setScanToggle(atoi(scan_toggle));

    // Advertise 주기 설정 읽기
    char adv_interval[10];
    readEEPROM(EEPROM_ADDR_INTV, adv_interval);
    config.setAdvInterval(atoi(adv_interval));

    // 최대 송신 전력 읽기
    char transmit_power[10];
    readEEPROM(EEPROM_ADDR_TXPW, transmit_power);
    config.setTransmitPower(atoi(transmit_power));
}
