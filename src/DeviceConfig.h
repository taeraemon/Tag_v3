#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

#include <Arduino.h>
#include <cstring>

class DeviceConfig {
private:
    char ssid[100];
    char pswd[100];
    int scan_toggle;
    int adv_interval;
    int transmit_power;
    char server_ip[150];    // 서버 IP 저장
    int server_port;        // 포트 번호 저장

public:
    static DeviceConfig& getInstance() {
        static DeviceConfig instance;
        return instance;
    }

    // getter/setter for ssid
    void setSSID(const char* newSSID) {
        strcpy(ssid, newSSID);
    }
    const char* getSSID() {
        return ssid;
    }

    // getter/setter for pswd
    void setPassword(const char* newPassword) {
        strcpy(pswd, newPassword);
    }
    const char* getPassword() {
        return pswd;
    }

    // getter/setter for scan toggle
    void setScanToggle(int toggle) {
        scan_toggle = toggle;
    }
    int getScanToggle() {
        return scan_toggle;
    }

    // getter/setter for adv_interval
    void setAdvInterval(int interval) {
        adv_interval = interval;
    }
    int getAdvInterval() {
        return adv_interval;
    }

    // getter/setter for transmit_power
    void setTransmitPower(int power) {
        transmit_power = power;
    }
    int getTransmitPower() {
        return transmit_power;
    }

    // getter/setter for server IP
    void setServerIP(const char* newIP) {
        strcpy(server_ip, newIP);
    }
    const char* getServerIP() {
        return server_ip;
    }

    // getter/setter for server port
    void setServerPort(int port) {
        server_port = port;
    }
    int getServerPort() {
        return server_port;
    }

private:
    DeviceConfig() : scan_toggle(1), adv_interval(100), transmit_power(80), server_port(55552) {
        strcpy(ssid, "default_ssid");
        strcpy(pswd, "default_pswd");
        strcpy(server_ip, "111.118.38.151");
    }

    // 복사 생성자 금지
    DeviceConfig(const DeviceConfig&) = delete;
    void operator=(const DeviceConfig&) = delete;
};

#endif // DEVICE_CONFIG_H
