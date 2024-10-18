#include <Arduino.h>
#include "Battery_monitor.h"

int batteryMeasurements[NUM_MEASUREMENTS] = {0};  // 측정값 저장 배열
int currentIndex = 0;  // 현재 인덱스
int measurementCount = 0;  // 실제 측정된 값의 개수

// 배터리 모니터링 초기화 함수
void initBatteryMonitor() {
    pinMode(PIN_BATTERY_MON, INPUT);  // 배터리 핀을 입력 모드로 설정
    pinMode(PIN_BATTERY_LED, OUTPUT);
    analogReadResolution(12);     // 아날로그 입력 해상도를 12비트로 설정
}

// 최근 10개의 배터리 전압 측정값의 평균을 계산하는 함수
int getAverageBatteryLevel() {
    int currentVoltage = 2 * analogReadMilliVolts(PIN_BATTERY_MON);  // 현재 배터리 전압 측정 (mV 단위)
    batteryMeasurements[currentIndex] = currentVoltage;      // 측정값 배열에 저장
    currentIndex = (currentIndex + 1) % NUM_MEASUREMENTS;    // 배열 인덱스 순환

    if (measurementCount < NUM_MEASUREMENTS) {
        measurementCount++;  // 측정된 값의 개수 증가
    }

    // 최근 측정값들의 평균 계산
    int volt_sum = 0;
    for (int i = 0; i < measurementCount; i++) {
        volt_sum += batteryMeasurements[i];
    }
    int volt_avg = volt_sum / measurementCount;  // 이동평균 계산

    // 측정된 평균 전압 범위 제한 (3.0V ~ 4.2V)
    if (volt_avg > 4200) {
        volt_avg = 4200;
    }
    else if (volt_avg < 3000) {
        volt_avg = 3000;
    }

    // 전압을 퍼센트로 변환 (3.0V ~ 4.2V 기준)
    int batteryPercentage = map(volt_avg, 3000, 4200, 0, 100);

    // 배터리가 30% 미만일 경우 LED를 점멸
    if (batteryPercentage <= 30) {
        digitalWrite(PIN_BATTERY_LED, !digitalRead(PIN_BATTERY_LED));
        Serial.println("Not enough Battery");
    }
    else {
        digitalWrite(PIN_BATTERY_LED, LOW);
    }

    return batteryPercentage;
}
