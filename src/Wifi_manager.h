#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>

void StartWiFi();
void ScanAndSend();
bool isScanEnabled();

#endif // WIFI_MANAGER_H
