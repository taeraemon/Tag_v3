#include <Arduino.h>
#include "Transmission_manager.h"
#include "RTC_manager.h"
#include "LTE_manager.h"
#include "WiFi_manager.h"
#include "DeviceConfig.h"

// TCP 연결 및 데이터 수집 확인 함수
void transmitData() {
    // RTC에서 UnixTime 가져오기
    uint32_t unixTime = getTime();  // RTC_manager에서 제공하는 함수

    // LTE 및 WiFi 데이터 가져오기
    LTEInfo lteData = getLTEData();
    LTENeighbourCellInfo* lteNeighbours;
    int neighbourCount = 0;
    getLTENeighbourCells(&lteNeighbours, &neighbourCount);

    WiFiInfo* wifiData;
    int wifiCount = 0;
    getWiFiData(&wifiData, &wifiCount);  // WiFi_manager에서 제공하는 함수

    // 패킷화
    uint8_t packet[1024];
    int packetLength = 0;

    // UnixTime 정보 패킷화 (4 bytes)
    packet[packetLength++] = (unixTime >> 24) & 0xFF;
    packet[packetLength++] = (unixTime >> 16) & 0xFF;
    packet[packetLength++] = (unixTime >> 8) & 0xFF;
    packet[packetLength++] = unixTime & 0xFF;

    // LTE Serving Cell 정보 패킷화
    packet[packetLength++] = (lteData.cid >> 24) & 0xFF;
    packet[packetLength++] = (lteData.cid >> 16) & 0xFF;
    packet[packetLength++] = (lteData.cid >> 8) & 0xFF;
    packet[packetLength++] = lteData.cid & 0xFF;
    packet[packetLength++] = (lteData.pci >> 8) & 0xFF;
    packet[packetLength++] = lteData.pci & 0xFF;
    packet[packetLength++] = lteData.band;
    packet[packetLength++] = (lteData.mnc >> 8) & 0xFF;
    packet[packetLength++] = lteData.mnc & 0xFF;
    packet[packetLength++] = lteData.rsrp;
    packet[packetLength++] = lteData.rsrq;

    // LTE 인접 셀 정보 패킷화
    for (int i = 0; i < neighbourCount; i++) {
        packet[packetLength++] = lteNeighbours[i].isIntra ? 0x01 : 0x00;
        packet[packetLength++] = (lteNeighbours[i].cid >> 24) & 0xFF;
        packet[packetLength++] = (lteNeighbours[i].cid >> 16) & 0xFF;
        packet[packetLength++] = (lteNeighbours[i].cid >> 8) & 0xFF;
        packet[packetLength++] = lteNeighbours[i].cid & 0xFF;
        packet[packetLength++] = (lteNeighbours[i].pci >> 8) & 0xFF;
        packet[packetLength++] = lteNeighbours[i].pci & 0xFF;
        packet[packetLength++] = lteNeighbours[i].rsrp;
        packet[packetLength++] = lteNeighbours[i].rsrq;
    }

    // WiFi 정보 패킷화
    for (int i = 0; i < wifiCount; i++) {
        memcpy(&packet[packetLength], wifiData[i].mac, 6);
        packetLength += 6;
        packet[packetLength++] = wifiData[i].rssi;
    }

    // TCP 연결 및 전송
    // connectTCP();
    // sendPacket(packet, packetLength);
    disconnectTCP();

    printScanResults();

    // 데이터 초기화
    clearLTEData();
    clearWiFiData();
}

// TCP 연결 함수
bool connectTCP() {
    Serial2.write("AT+QIOPEN=1,0,\"TCP\",\"");
    Serial2.write(SERVER_IP);  // Transmission_manager.h에서 정의된 SERVER_IP 사용
    Serial2.write("\",");
    Serial2.print(SERVER_PORT);  // Transmission_manager.h에서 정의된 SERVER_PORT 사용
    Serial2.write(",0,0\r\n");
    delay(1000);  // 첫 번째 명령 후 대기
    return true;  // 연결 상태는 확인하지 않고 항상 true 반환
}

// 데이터 전송 함수
void sendPacket(uint8_t* packet, int length) {
    char sendCommand[32];  // AT+QISEND 명령을 저장할 버퍼
    sprintf(sendCommand, "AT+QISEND=0,%d\r\n", length);  // 문자열 생성

    Serial2.write(sendCommand);  // 한번에 write
    delay(500);  // 명령 전송 후 대기

    Serial2.write(packet, length);  // 실제 데이터 전송
    Serial.println(length);  // 전송한 데이터 길이를 출력
    Serial2.write("\r\n");
    delay(500);
    
    Serial.println("Data Send Complete");
}

// TCP 연결 해제 함수
void disconnectTCP() {
    Serial2.write("AT+QICLOSE=0\r\n");
    delay(500);  // 세 번째 명령 후 대기
    Serial.println("TCP close");
}

void printScanResults() {
    // RTC에서 UnixTime 가져오기
    uint32_t unixTime = getTime();  // RTC_manager에서 제공하는 함수

    // LTE 및 WiFi 데이터 가져오기
    LTEInfo lteData = getLTEData();
    LTENeighbourCellInfo* lteNeighbours;
    int neighbourCount = 0;
    getLTENeighbourCells(&lteNeighbours, &neighbourCount);

    WiFiInfo* wifiData;
    int wifiCount = 0;
    getWiFiData(&wifiData, &wifiCount);

    // UnixTime 출력
    Serial.print("UnixTime: ");
    Serial.println(unixTime);

    // LTE Serving Cell 정보 출력
    Serial.println("LTE Serving Cell Info:");
    Serial.print("CID: "); Serial.println(lteData.cid);
    Serial.print("PCI: "); Serial.println(lteData.pci);
    Serial.print("Band: "); Serial.println(lteData.band);
    Serial.print("MNC: "); Serial.println(lteData.mnc);
    Serial.print("RSRP: "); Serial.println(lteData.rsrp);
    Serial.print("RSRQ: "); Serial.println(lteData.rsrq);

    // LTE 인접 셀 정보 출력
    Serial.println("LTE Neighbour Cells Info:");
    for (int i = 0; i < neighbourCount; i++) {
        Serial.print("Neighbour "); Serial.print(i + 1); Serial.println(":");
        Serial.print("Intra: "); Serial.println(lteNeighbours[i].isIntra ? "Yes" : "No");
        Serial.print("CID: "); Serial.println(lteNeighbours[i].cid);
        Serial.print("PCI: "); Serial.println(lteNeighbours[i].pci);
        Serial.print("RSRP: "); Serial.println(lteNeighbours[i].rsrp);
        Serial.print("RSRQ: "); Serial.println(lteNeighbours[i].rsrq);
    }

    // WiFi 정보 출력
    Serial.println("WiFi Info:");
    for (int i = 0; i < wifiCount; i++) {
        Serial.print("WiFi "); Serial.print(i + 1); Serial.println(":");
        Serial.print("MAC: ");
        for (int j = 0; j < 6; j++) {
            Serial.print(wifiData[i].mac[j], HEX);
            if (j < 5) Serial.print(":");
        }
        Serial.println();
        Serial.print("RSSI: "); Serial.println(wifiData[i].rssi);
    }

    Serial.println("------------ Scan Complete ------------");
}
