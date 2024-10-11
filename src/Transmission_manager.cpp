#include <Arduino.h>
#include "Transmission_manager.h"
#include "LTE_manager.h"
#include "WiFi_manager.h"
#include "DeviceConfig.h"

// TCP 연결 및 데이터 수집 확인 함수
void transmitData() {
    // LTE 및 WiFi 데이터 가져오기
    LTEInfo lteData = getLTEData();  // LTE_manager에서 제공하는 함수
    LTENeighbourCellInfo* lteNeighbours;
    int neighbourCount = 0;
    getLTENeighbourCells(&lteNeighbours, &neighbourCount);  // 인접 셀 정보 가져오기

    WiFiInfo* wifiData;
    int wifiCount = 0;
    getWiFiData(&wifiData, &wifiCount);  // WiFi_manager에서 제공하는 함수

    // 패킷화 (일단 데이터 확인을 위해 주석화된 상태로 두겠습니다)
    uint8_t packet[1024];  // 패킷 크기는 필요한 만큼 조정
    int packetLength = 0;

    // LTE Serving Cell 정보 패킷화 (예: 11 bytes)
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
    connectTCP();  // 소켓을 열고
    sendPacket(packet, packetLength);  // 패킷 전송
    disconnectTCP();  // 소켓을 닫음

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
