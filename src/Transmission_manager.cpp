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
    uint8_t packet[2048];  // 버퍼 크기 설정
    int packetLength = 0;

    // 1. 수집 시간 (UnixTime, 4바이트)
    packet[packetLength++] = (unixTime >> 24) & 0xFF;
    packet[packetLength++] = (unixTime >> 16) & 0xFF;
    packet[packetLength++] = (unixTime >> 8) & 0xFF;
    packet[packetLength++] = unixTime & 0xFF;

    // 2. 총 바이트 수 (LTE 신호와 WiFi 신호의 총 바이트 수)
    uint16_t totalBytes = 2 + (11 * (1 + neighbourCount)) + (7 * wifiCount);  // LTE 1개 신호 + 인접 셀, WiFi 신호 수
    packet[packetLength++] = (totalBytes >> 8) & 0xFF;
    packet[packetLength++] = totalBytes & 0xFF;

    // 3. LTE 신호 수 (1바이트)
    packet[packetLength++] = 1 + neighbourCount;  // LTE 신호 수: 1개 + 인접 셀

    // 4. LTE Serving Cell 정보 (11바이트)
    packet[packetLength++] = (lteData.cid >> 24) & 0xFF;   // CID 상위 바이트
    packet[packetLength++] = (lteData.cid >> 16) & 0xFF;
    packet[packetLength++] = (lteData.cid >> 8) & 0xFF;
    packet[packetLength++] = lteData.cid & 0xFF;           // CID 하위 바이트
    packet[packetLength++] = (lteData.pci >> 8) & 0xFF;    // PCI 상위 바이트
    packet[packetLength++] = lteData.pci & 0xFF;           // PCI 하위 바이트
    packet[packetLength++] = (lteData.band >> 8) & 0xFF;   // Band 상위 바이트 (2바이트 처리)
    packet[packetLength++] = lteData.band & 0xFF;          // Band 하위 바이트
    packet[packetLength++] = lteData.mnc;                  // MNC
    packet[packetLength++] = lteData.rsrp;                 // RSRP
    packet[packetLength++] = lteData.rsrq;                 // RSRQ

    // 5. LTE 인접 셀 정보 (신호당 11바이트)
    for (int i = 0; i < neighbourCount; i++) {
        packet[packetLength++] = (lteNeighbours[i].cid >> 24) & 0xFF;   // CID 상위 바이트
        packet[packetLength++] = (lteNeighbours[i].cid >> 16) & 0xFF;
        packet[packetLength++] = (lteNeighbours[i].cid >> 8) & 0xFF;
        packet[packetLength++] = lteNeighbours[i].cid & 0xFF;           // CID 하위 바이트
        packet[packetLength++] = (lteNeighbours[i].pci >> 8) & 0xFF;    // PCI 상위 바이트
        packet[packetLength++] = lteNeighbours[i].pci & 0xFF;           // PCI 하위 바이트
        packet[packetLength++] = (lteNeighbours[i].band >> 8) & 0xFF;   // Band 상위 바이트 (2바이트 처리)
        packet[packetLength++] = lteNeighbours[i].band & 0xFF;          // Band 하위 바이트
        packet[packetLength++] = lteNeighbours[i].mnc;                  // MNC
        packet[packetLength++] = lteNeighbours[i].rsrp;                 // RSRP
        packet[packetLength++] = lteNeighbours[i].rsrq;                 // RSRQ
    }

    // 6. WiFi 신호 수 (1바이트)
    packet[packetLength++] = wifiCount;

    // 7. WiFi 신호 정보 (신호당 7바이트)
    for (int i = 0; i < wifiCount; i++) {
        memcpy(&packet[packetLength], wifiData[i].mac, 6);  // MAC 주소
        packetLength += 6;
        packet[packetLength++] = wifiData[i].rssi;  // RSSI
    }

    // TCP 연결 및 전송
    if (connectTCP()) {
        sendPacket(packet, packetLength);
        disconnectTCP();
    }

    printScanResults();

    // 데이터 초기화
    clearLTEData();
    clearWiFiData();
}

// HTTP 연결 및 데이터 수집 확인 함수
void transmitDataHTTP() {
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

    // 1. DeviceConfig에서 SSID 가져와서 Tag로 사용
    DeviceConfig& config = DeviceConfig::getInstance();
    const char* tag = config.getSSID();  // getSSID()로 SSID 가져오기

    // 2. HTTP 데이터 문자열 준비
    char data[4096];  // 전송할 데이터 버퍼
    int length = 0;

    // 3. 수집 시간 추가 (UnixTime)
    length += sprintf(data + length, "sig=%lu|", unixTime);

    // 4. MNC 추가 (통신사 코드)
    length += sprintf(data + length, "%d|", lteData.mnc);

    // 5. LTE 신호 수 추가 (서빙 셀 + 인접 셀 수)
    length += sprintf(data + length, "%d|", 1 + neighbourCount);

    // 6. LTE Serving Cell 정보 추가 (형식: PCI_BAND_0_CID[rsrp/rsrq])
    length += sprintf(data + length, "%d_%d_0_%d[%d/%d],", lteData.pci, lteData.band, lteData.cid, lteData.rsrp, lteData.rsrq);

    // 7. LTE Neighbour Cell 정보 추가 (형식: PCI_BAND_2_CID[rsrp/rsrq])
    for (int i = 0; i < neighbourCount; i++) {
        length += sprintf(data + length, "%d_%d_2_%d[%d/%d],",
                          lteNeighbours[i].pci, lteNeighbours[i].band, lteNeighbours[i].cid, lteNeighbours[i].rsrp, lteNeighbours[i].rsrq);
    }
    
    // 마지막 쉼표 제거
    if (neighbourCount > 0) {
        length--;  // 쉼표를 덮어씀
    }

    // 8. WiFi 신호 수 추가
    length += sprintf(data + length, "|%d|", wifiCount);

    // 9. WiFi 신호 정보 추가 (형식: MAC[rssi])
    for (int i = 0; i < wifiCount; i++) {
        length += sprintf(data + length, "%02x%02x%02x%02x%02x%02x[%d],",
                          wifiData[i].mac[0], wifiData[i].mac[1], wifiData[i].mac[2],
                          wifiData[i].mac[3], wifiData[i].mac[4], wifiData[i].mac[5],
                          wifiData[i].rssi);
    }

    // 마지막 쉼표 제거
    if (wifiCount > 0) {
        length--;  // 쉼표를 덮어씀
    }
    data[length] = '\0';    // 널문자 추가

    // HTTP POST 요청할 패킷 만들기
    char postData[2048];
    sprintf(postData, "tag=%s&%s", tag, data);

    Serial.println(postData);
    Serial.println(strlen(postData));



    // 서버 IP와 포트 설정 (DeviceConfig에서 가져옴)
    const char* serverURL = config.getServerIP();

    // // AT 명령어 구성
    Serial.println("AT+QHTTPCFG=\"contextid\",1");
    delay(250);

    Serial2.print("AT+QHTTPURL=");
    Serial2.print(strlen(serverURL));
    Serial2.println(",30");  // URL 입력 대기 시간
    delay(500);

    Serial2.println(serverURL);    // URL 전송
    delay(500);

    // 패킷 전송 명령어
    Serial2.print("AT+QHTTPPOST=");
    Serial2.print(strlen(postData));
    Serial2.println(",30");  // 데이터 전송 대기 시간
    delay(500);

    Serial2.println(postData);  // 패킷 데이터 전송
    delay(500);

    // HTTP 전송 완료 명령어
    Serial2.println("AT+QHTTPSTOP");
    delay(250);



    // 데이터 초기화
    clearLTEData();
    clearWiFiData();
}

// TCP 연결 함수
bool connectTCP() {
    DeviceConfig& config = DeviceConfig::getInstance();
    
    // 서버 IP와 포트 가져오기
    const char* server_ip = config.getServerIP();
    int server_port = config.getServerPort();

    // TCP 연결 명령 생성
    Serial2.write("AT+QIOPEN=1,0,\"TCP\",\"");
    Serial2.write(server_ip);  // EEPROM에서 가져온 IP 사용
    Serial2.write("\",");
    Serial2.print(server_port);  // EEPROM에서 가져온 포트 사용
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
        Serial.print("CID: "); Serial.println(lteNeighbours[i].cid);
        Serial.print("PCI: "); Serial.println(lteNeighbours[i].pci);
        Serial.print("Band: "); Serial.println(lteNeighbours[i].band);
        Serial.print("MNC: "); Serial.println(lteNeighbours[i].mnc);
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
