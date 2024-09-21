#include <Arduino.h>
#include "RTC_manager.h"

RTC_DS3231 rtc;

// RTC 초기화 함수
void initRTC() {
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
        while (true);  // RTC가 없으면 무한 대기
    }

    if (rtc.lostPower()) {
        resetRTC();
    }
}

// RTC 시간이 날아갔을 때 빌드 타임으로 복구
void resetRTC() {
    DateTime curr_kst = DateTime(F(__DATE__), F(__TIME__));
    DateTime curr_utc = curr_kst - TimeSpan(9 * 3600);

    rtc.adjust(curr_utc);  // 컴파일 시의 시간을 설정
}

// Unix 타임을 사용하여 RTC 시간 설정
void setTime(int unixTime) {
    DateTime newTime(unixTime);
    rtc.adjust(newTime);
    Serial.println("Time set successfully");
}

// 현재 Unix 타임(초 단위)을 반환
int getTime() {
    DateTime now = rtc.now();
    return now.unixtime();  // Unix 타임 반환
}

// 현재 시간을 출력
void printTime() {
    DateTime now = rtc.now();

    Serial.print(now.year(),   DEC);
    Serial.print('/');
    Serial.print(now.month(),  DEC);
    Serial.print('/');
    Serial.print(now.day(),    DEC);
    Serial.print(" ");
    Serial.print(now.hour(),   DEC);
    Serial.print(':');
    Serial.print(now.minute(), DEC);
    Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println(" (UTC)");

    // 한국 시간(KST) 출력
    DateTime kst = now + TimeSpan(9 * 3600);  // UTC + 9시간 (한국 시간)
    Serial.print(kst.year(),   DEC);
    Serial.print('/');
    Serial.print(kst.month(),  DEC);
    Serial.print('/');
    Serial.print(kst.day(),    DEC);
    Serial.print(" ");
    Serial.print(kst.hour(),   DEC);
    Serial.print(':');
    Serial.print(kst.minute(), DEC);
    Serial.print(':');
    Serial.print(kst.second(), DEC);
    Serial.println(" (KST)");
}
