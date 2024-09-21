#ifndef RTC_MANAGER_H
#define RTC_MANAGER_H

#include <RTClib.h>

// 함수 선언
void initRTC();
void resetRTC();
void setTime(int unixTime);
void printTime();
int getTime();

#endif // RTC_MANAGER_H
