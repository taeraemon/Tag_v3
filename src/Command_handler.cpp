#include "Command_handler.h"
#include "EEPROM_manager.h"
#include "WiFi_manager.h"
#include <Arduino.h>

// 전역 변수 참조

void processCommand(const std::string &command) {
    // Serial.print("Received Command: ");
    // Serial.println(command.c_str());

    // if (command.length() > 0) {
    //     char cmd[100] = {0};
    //     strcpy(cmd, command.c_str());

    //     // SCN - WiFi 스캔 토글
    //     if (cmd[0] == '1') {
    //         if (cmd[1] == '0') {
    //             scan_toggle = 0;
    //             writeEEPROM(100, (char *)"0");
    //             Serial.println("WiFi scan disabled");
    //         } else if (cmd[1] == '1') {
    //             scan_toggle = 1;
    //             writeEEPROM(100, (char *)"1");
    //             Serial.println("WiFi scan enabled");
    //         }
    //     }
    //     // SSID 설정
    //     else if (cmd[0] == '2') {
    //         strcpy(ssid, &cmd[2]);
    //         writeEEPROM(0, ssid);
    //         Serial.print("SSID set to: ");
    //         Serial.println(ssid);
    //         StartWiFi();
    //     }
    //     // 기타 설정 (광고 주기 및 송신 전력 설정)
    //     else if (cmd[0] == '3') {
    //         int val = atoi(&cmd[2]);

    //         if (cmd[1] == '1') {
    //             adv_interval = val;
    //             Serial.print("Advertising interval set to: ");
    //             Serial.println(adv_interval);
    //         } else if (cmd[1] == '2') {
    //             transmit_power = val;
    //             Serial.print("Transmit power set to: ");
    //             Serial.println(transmit_power);
    //         }
    //     }
    //     // 배터리 모니터링 요청
    //     else if (cmd[0] == '4') {
    //         Serial.println("Battery monitor requested.");
    //     }
    //     // 시간 동기화 요청
    //     else if (cmd[0] == '5') {
    //         int newTime = atoi(&cmd[1]);
    //         Serial.print("Setting new time to: ");
    //         Serial.println(newTime);
    //     }
    //     // 디바이스 리셋
    //     else if (cmd[0] == '6') {
    //         Serial.println("Device reset requested.");
    //         resetEEPROM();
    //     }
    // }
}
