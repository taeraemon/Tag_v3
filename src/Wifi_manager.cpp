#include <Arduino.h>
#include "WiFi_manager.h"
#include "BLE_manager.h"
#include "DeviceConfig.h"

// WiFi 데이터 저장소
#define MAX_WIFI_COUNT 50
WiFiInfo wifiData[MAX_WIFI_COUNT];
int wifiCount = 0;

// WiFi 데이터 반환 함수
void getWiFiData(WiFiInfo** wifiList, int* count) {
    *wifiList = wifiData;
    *count = wifiCount;
}

// WiFi 데이터 초기화 함수
void clearWiFiData() {
    memset(wifiData, 0, sizeof(wifiData));
    wifiCount = 0;
}

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

// 주변 AP 스캔을 시작, 결과를 저장
void ScanAndSend() {
    wifiCount = WiFi.scanNetworks();  // 네트워크 스캔 시작

    if (wifiCount == 0) {
        Serial.println("No networks found.");
    }
    else {
        Serial.printf("%d networks found:\n", wifiCount);

        // 스캔된 네트워크 정보를 wifiData 배열에 저장
        for (int i = 0; i < wifiCount && i < MAX_WIFI_COUNT; i++) {
            String ssid_str = WiFi.SSID(i);
            strncpy(wifiData[i].ssid, ssid_str.c_str(), sizeof(wifiData[i].ssid) - 1);
            wifiData[i].ssid[sizeof(wifiData[i].ssid) - 1] = '\0';  // null terminate

            // MAC 주소 저장
            uint8_t* macAddr = WiFi.BSSID(i);  // MAC 주소 가져오기
            memcpy(wifiData[i].mac, macAddr, 6);  // 6바이트 MAC 주소 복사

            wifiData[i].rssi = WiFi.RSSI(i);
        }
    }
}

// 스캔 토글 확인
bool isScanEnabled() {
    // DeviceConfig에서 스캔 토글 값 가져오기
    DeviceConfig& config = DeviceConfig::getInstance();
    return config.getScanToggle() == 1;  // 스캔 토글이 1이면 활성화된 상태
}

// WiFi beacon interval 설정 함수
void setWiFiBeaconInterval(int interval) {
    wifi_config_t config_w;

    // AP 모드 설정을 위한 beacon_interval 설정
    config_w.ap.beacon_interval = interval;  // Beacon interval 설정

    // AP 설정 적용
    esp_wifi_set_config(WIFI_IF_AP, &config_w);

    Serial.print("WiFi beacon interval set to: ");
    Serial.println(interval);
}

// 0~9 값을 받아 WiFi 송신 전력 설정 함수
void adjustWiFiTransmitPower(int level) {
    int wifiPower = map(level, 0, 9, 8, 80);  // 8 ~ 80 (2 dBm ~ 20 dBm), 0.25dbm per step
    esp_wifi_set_max_tx_power(wifiPower);

    Serial.print("WiFi transmit power adjusted to: ");
    Serial.println(wifiPower);
}
