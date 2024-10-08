#ifndef LTEMANAGER_H
#define LTEMANAGER_H

#include <Arduino.h>

void LTEmanager_init();
void LTEmanager_sendATCommand(const char* command);
void LTEmanager_readSerialBuffer();
int endsWithOK();
void LTEmanager_decodePacket();
bool LTEmanager_readSerialBufferContains(const char* target);
void connectTCP();
void sendBinaryPacket();

#endif // LTEMANAGER_H
