#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

#define PIN_BATTERY_MON A0   // 배터리 전압 측정 핀
#define PIN_BATTERY_LED 33   // 배터리 상태 LED
#define NUM_MEASUREMENTS 10  // 측정값 배열 크기 (이동평균 대상)

void initBatteryMonitor();        // 배터리 모니터 초기화 함수
int getAverageBatteryLevel();     // 배터리 전압 측정 및 이동평균 계산 함수

#endif // BATTERY_MONITOR_H
