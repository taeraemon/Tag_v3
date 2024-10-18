#ifndef TRANSMISSION_MANAGER_H
#define TRANSMISSION_MANAGER_H

#include <Arduino.h>
#include "DeviceConfig.h"

void transmitData();
void transmitDataHTTP();
bool connectTCP();         // TCP 연결 함수
void sendPacket(uint8_t* packet, int length);  // 패킷 전송 함수
void disconnectTCP();      // TCP 연결 해제 함수
void printScanResults();

#endif // TRANSMISSION_MANAGER_H
