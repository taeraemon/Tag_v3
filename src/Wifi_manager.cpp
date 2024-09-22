#include <Arduino.h>
#include "WiFi_manager.h"
#include "BLE_manager.h"
#include "DeviceConfig.h"

// Wifi 인스턴스 시작. 재호출 시 ssid 업데이트 되어 시작
void StartWiFi() {
    // DeviceConfig에서 SSID와 비밀번호 가져오기
    DeviceConfig& config = DeviceConfig::getInstance();
    const char* ssid = config.getSSID();
    const char* pswd = config.getPassword();

    // WiFi Access Point 시작
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, pswd);

    // WiFi AP 시작 후 정보 출력
    Serial.print("WiFi AP started with SSID: ");
    Serial.println(ssid);
}

// 주변 AP 스캔을 시작, BLE로 전송
void ScanAndSend() {
    digitalWrite(LED_BUILTIN, HIGH);
    int n = WiFi.scanNetworks();
    digitalWrite(LED_BUILTIN, LOW);

    if (n == 0) {
        Serial.println("No networks found.");
    } else {
        Serial.printf("%d networks found:\n", n);
        for (int i = 0; i < n; i++) {
            String foundSSID_str = WiFi.SSID(i);
            const char* foundSSID = foundSSID_str.c_str();
            int rssi = WiFi.RSSI(i);
            notifyWiFiStatus(foundSSID, rssi);
            delay(10);
        }
    }
}

// 스캔 토글 확인
bool isScanEnabled() {
    // DeviceConfig에서 스캔 토글 값 가져오기
    DeviceConfig& config = DeviceConfig::getInstance();
    return config.getScanToggle() == 1;  // 스캔 토글이 1이면 활성화된 상태
}
