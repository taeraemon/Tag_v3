#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include <Arduino.h>
#include <string>

#define CMD_SCAN    '1'
#define CMD_SSID    '2'
#define CMD_CONFIG  '3'
#define CMD_BATTERY '4'
#define CMD_TIME    '5'
#define CMD_RESET   '6'

void processCommand(const std::string &command);
void handleScanCommand(const char* cmd);
void handleSSIDCommand(const char* cmd);
void handleConfigCommand(const char* cmd);
void handleBatteryCommand();
void handleTimeCommand(const char* cmd);
void handleResetCommand();

#endif // COMMAND_HANDLER_H
