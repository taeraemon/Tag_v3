
/*
1. EEPROM 초기화
EEPROM_DFLT에 정의된 값으로 초기화

2. RTC 초기화
컴파일 타임의 UTC를 rtc에 저장
rtc를 활성화, unix time을 컴파일 타임으로 설정
*/

#include <Arduino.h>
#include "EEPROM_manager.h"
#include "RTC_manager.h"

unsigned long previousMillis = 0;
const long interval = 5000;  // 5초 간격

void setup() {
    Serial.begin(115200);

    // EEPROM Initialize
    initEEPROM();
    resetEEPROM();

    char buffer[1024];
    readEEPROM(EEPROM_ADDR_SSID, buffer);
    Serial.print("Written SSID : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_PSWD, buffer);
    Serial.print("Written PSWD : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_SCAN, buffer);
    Serial.print("Written SCAN : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_INTV, buffer);
    Serial.print("Written INTV : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_TXPW, buffer);
    Serial.print("Written TXPW : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_SEVR, buffer);
    Serial.print("Written SEVR : "); Serial.println(buffer);
    readEEPROM(EEPROM_ADDR_PORT, buffer);
    Serial.print("Written PORT : "); Serial.println(buffer);

    // RTC Initialize
    initRTC();
    resetRTC();
    printTime();
}

void loop() {
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;

        // 5초마다 EEPROM 및 RTC 정보 출력
        char buffer[100];
        readEEPROM(EEPROM_ADDR_SSID, buffer);
        Serial.print("Written SSID : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_PSWD, buffer);
        Serial.print("Written PSWD : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_SCAN, buffer);
        Serial.print("Written SCAN : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_INTV, buffer);
        Serial.print("Written INTV : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_TXPW, buffer);
        Serial.print("Written TXPW : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_SEVR, buffer);
        Serial.print("Written SEVR : "); Serial.println(buffer);
        readEEPROM(EEPROM_ADDR_PORT, buffer);
        Serial.print("Written PORT : "); Serial.println(buffer);

        printTime();
    }
}
