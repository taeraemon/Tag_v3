#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>

#define EEPROM_SIZE      300
#define EEPROM_ADDR_SSID 0
#define EEPROM_ADDR_PSWD 50
#define EEPROM_ADDR_SCAN 100
#define EEPROM_ADDR_INTV 110
#define EEPROM_ADDR_TXPW 120

bool initEEPROM();
void writeEEPROM(int addr, const char* data);
void readEEPROM(int addr, char* data);
void resetEEPROM();
void loadDeviceConfigFromEEPROM();

#endif // EEPROM_MANAGER_H
