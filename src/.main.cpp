
/*
1. EEPROM 초기화
ssid : default
pswd : 00000000
scan : on (1)
interval(scan) : 5000
transmit power : 80

2. RTC 초기화
컴파일 타임의 UTC를 rtc에 저장
rtc를 활성화, unix time을 컴파일 타임으로 설정
*/

#include <Arduino.h>
#include "EEPROM_manager.h"
#include "RTC_manager.h"

void setup() {
    Serial.begin(115200);



    // EEPROM Initialize
    initEEPROM();
    resetEEPROM();

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



    // RTC Initialize
    initRTC();
    resetRTC();
    printTime();
}

void loop() {
    
}
