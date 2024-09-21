#include "Battery_monitor.h"
#include <Arduino.h>

int batteryMeasurements[NUM_MEASUREMENTS] = {0};
int currentIndex = 0;
int measurementCount = 0;  // 실제 측정된 값의 개수

// 배터리 모니터링 초기화
void initBatteryMonitor() {
    pinMode(PIN_BATTERY, INPUT);
    analogReadResolution(12);  // 아날로그 입력 해상도 설정
}

// 최근 10개의 배터리 전압 측정값의 평균을 계산하는 함수
int getAverageBatteryLevel() {
    int currentVoltage = analogReadMilliVolts(PIN_BATTERY);  // 현재 전압 측정
    batteryMeasurements[currentIndex] = currentVoltage;      // 배열에 저장
    currentIndex = (currentIndex + 1) % NUM_MEASUREMENTS;    // 인덱스를 순환

    // 측정된 값의 개수 증가 (최대 NUM_MEASUREMENTS까지만 증가)
    if (measurementCount < NUM_MEASUREMENTS) {
        measurementCount++;
    }

    // 최근 10개의 측정값 평균 계산
    int volt_sum = 0;
    for (int i = 0; i < measurementCount; i++) {
        volt_sum += batteryMeasurements[i];
    }
    int volt_avg = volt_sum / measurementCount;

    // 전압 범위 초과에 대한 처리 (3.0V ~ 4.2V)
    if (volt_avg > 4200) {
        volt_avg = 4200;  // 최대 4.2V로 제한
    } else if (volt_avg < 3000) {
        volt_avg = 3000;  // 최소 3.0V로 제한
    }

    // 전압을 퍼센트로 변환 (3.0V ~ 4.2V 기준)
    int batteryPercentage = map(volt_avg, 3000, 4200, 0, 100);

    // Serial로 현재 배터리 상태 출력
    Serial.print("Recent Average Battery voltage: ");
    Serial.print(volt_avg);
    Serial.print(" mV, Battery Level: ");
    Serial.print(batteryPercentage);
    Serial.println("%");

    return batteryPercentage;
}
