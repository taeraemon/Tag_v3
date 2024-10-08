#ifndef EEPROM_MANAGER_H
#define EEPROM_MANAGER_H

#include <Arduino.h>

#define EEPROM_SIZE      300

#define EEPROM_ADDR_SSID 0
#define EEPROM_ADDR_PSWD 50
#define EEPROM_ADDR_SCAN 100
#define EEPROM_ADDR_INTV 110
#define EEPROM_ADDR_TXPW 120
#define EEPROM_ADDR_SEVR 
#define EEPROM_ADDR_PORT 
#define EEPROM_ADDR_UUID 

#define EEPROM_DFLT_SSID "default"
#define EEPROM_DFLT_PSWD "00000000"
#define EEPROM_DFLT_SCAN "1"
#define EEPROM_DFLT_INTV "10000"
#define EEPROM_DFLT_TXPW "9"
#define EEPROM_DFLT_SEVR "111.118.38.151"
#define EEPROM_DFLT_PORT "55555"
#define EEPROM_DFLT_UUID "1234"

bool initEEPROM();
void writeEEPROM(int addr, const char* data);
void readEEPROM(int addr, char* data);
void resetEEPROM();
void loadDeviceConfigFromEEPROM();

#endif // EEPROM_MANAGER_H
