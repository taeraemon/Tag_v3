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

    // 데이터 확인용 출력
    Serial.println("=== LTE Serving Cell Data ===");
    Serial.print("CID: 0x"); Serial.println(lteData.cid, HEX);
    Serial.print("PCI: "); Serial.println(lteData.pci);
    Serial.print("Band: "); Serial.println(lteData.band);
    Serial.print("MNC: "); Serial.println(lteData.mnc);
    Serial.print("RSRP: "); Serial.println(lteData.rsrp);
    Serial.print("RSRQ: "); Serial.println(lteData.rsrq);

    Serial.println("=== LTE Neighbour Cells ===");
    for (int i = 0; i < neighbourCount; i++) {
        Serial.print(lteNeighbours[i].isIntra ? "Intra" : "Inter");
        Serial.print(" Neighbour Cell CID: 0x"); Serial.println(lteNeighbours[i].cid, HEX);
        Serial.print("PCI: "); Serial.println(lteNeighbours[i].pci);
        Serial.print("RSRP: "); Serial.println(lteNeighbours[i].rsrp);
        Serial.print("RSRQ: "); Serial.println(lteNeighbours[i].rsrq);
    }

    Serial.println("=== WiFi Data ===");
    for (int i = 0; i < wifiCount; i++) {
        Serial.print("MAC: ");
        for (int j = 0; j < 6; j++) {
            Serial.print(wifiData[i].mac[j], HEX);
            if (j < 5) Serial.print(":");
        }
        Serial.print(" RSSI: "); Serial.println(wifiData[i].rssi);
    }

    // TCP 연결 및 전송 주석 처리
    /*
    if (connectTCP()) {
        sendPacket(packet, packetLength);
        disconnectTCP();
    } else {
        Serial.println("TCP 연결 실패");
    }
    */

    // 데이터 초기화
    clearLTEData();
    clearWiFiData();
}


// TCP 연결 함수
bool connectTCP() {
    Serial2.print("AT+QIOPEN=1,0,\"TCP\",\"");
    Serial2.print(SERVER_IP);
    Serial2.print("\",");
    Serial2.print(SERVER_PORT);
    Serial2.println(",0,1");
    delay(2000);  // 연결 대기

    // 연결 상태 확인
    Serial2.println("AT+QISTATE=0,0");  // 소켓 0의 상태 확인
    delay(1000);

    // 응답 확인
    if (LTE_manager_readSerialBufferContains("CONNECT OK")) {
        Serial.println("TCP 연결 성공");
        return true;
    } else {
        Serial.println("TCP 연결 실패");
        return false;
    }
}

// 데이터 전송 함수
void sendPacket(uint8_t* packet, int length) {
    // 데이터 전송 명령어
    Serial2.print("AT+QISEND=0,");
    Serial2.println(length);
    delay(500);  // 전송 준비 대기

    // 패킷 전송
    Serial2.write(packet, length);

    // 데이터 전송 완료를 위해 Ctrl+Z (0x1A) 전송
    Serial2.write(0x1A);

    Serial.println("데이터 전송 완료");
}

// TCP 연결 해제 함수
void disconnectTCP() {
    Serial2.println("AT+QICLOSE=0");
    delay(500);
    Serial.println("TCP 연결 해제");
}
