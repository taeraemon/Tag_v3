#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_wifi.h>

void StartWiFi();
void ScanAndSend();
bool isScanEnabled();
void setWiFiBeaconInterval(int interval);  // WiFi beacon interval 설정 함수
void adjustWiFiTransmitPower(int level);  // 0~9 값을 받아 WiFi 송신 전력 설정 함수

#endif // WIFI_MANAGER_H
