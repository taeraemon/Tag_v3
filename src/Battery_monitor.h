#ifndef BATTERY_MONITOR_H
#define BATTERY_MONITOR_H

#include <Arduino.h>

#define PIN_BATTERY A0
#define NUM_MEASUREMENTS 10

void initBatteryMonitor();
int getAverageBatteryLevel();

#endif // BATTERY_MONITOR_H
