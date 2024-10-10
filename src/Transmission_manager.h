#ifndef TRANSMISSION_MANAGER_H
#define TRANSMISSION_MANAGER_H

#include <Arduino.h>

#define SERVER_IP "111.118.38.151"
#define SERVER_PORT 55555

void transmitData();
bool connectTCP();         // TCP 연결 함수
void sendPacket(uint8_t* packet, int length);  // 패킷 전송 함수
void disconnectTCP();      // TCP 연결 해제 함수

#endif // TRANSMISSION_MANAGER_H
