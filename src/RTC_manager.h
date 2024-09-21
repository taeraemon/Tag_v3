#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <Arduino.h>
#include <RTClib.h>

void initRTC();
void resetRTC();
void setTime(int unixTime);
int getTime();
void printTime();

#endif // RTC_MANAGER_H
