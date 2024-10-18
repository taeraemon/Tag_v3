#include <Arduino.h>
#include "LTE_manager.h"
#include "DeviceConfig.h"

// 최대 인접 셀 수 설정
#define MAX_NEIGHBOUR_CELLS 50

LTEInfo lteServingCell;  // LTE Serving Cell 정보 저장
LTENeighbourCellInfo lteNeighbourCells[MAX_NEIGHBOUR_CELLS];  // LTE 인접 셀 배열
int neighbourCellCount = 0;  // 인접 셀의 개수

char buffer[2048];  // Serial buffer
int bufferIndex = 0;


void clearLTEData() {
    memset(&lteServingCell, 0, sizeof(LTEInfo));  // LTE 데이터 초기화
    memset(lteNeighbourCells, 0, sizeof(lteNeighbourCells));  // 인접 셀 데이터 초기화
    neighbourCellCount = 0;
}

void LTE_manager_sendATCommand(const char* command) {
    Serial2.write(command);  // Send the given command to the modem
}

void LTE_manager_readSerialBuffer() {
    // 시리얼 데이터 모두 읽어서 버퍼에 저장
    while (Serial2.available()) {
        char c = Serial2.read();

        if (bufferIndex < sizeof(buffer) - 1) {  // Check buffer capacity
            buffer[bufferIndex++] = c;  // Add char to buffer
            buffer[bufferIndex] = '\0'; // Null terminate string
        }
        else {
            // Buffer overflow protection: handle the case where buffer is full
            Serial.println("Warning: Buffer Overflow");
            bufferIndex = 0;  // Reset buffer to avoid overflow
            buffer[0] = '\0';
        }
    }

    // 'OK\r\n'으로 끝나는지 확인 후 패킷 디코딩
    if (endsWithOK()) {
        Serial.println(buffer);  // 버퍼 내용을 출력하여 디버깅
        LTE_manager_decodePacket();
        bufferIndex = 0;  // Clear buffer after decoding
        buffer[0] = '\0';
    }
}

// LTE 데이터 반환 함수
LTEInfo getLTEData() {
    return lteServingCell;
}

// LTE 인접 셀 데이터 반환 함수
void getLTENeighbourCells(LTENeighbourCellInfo** neighbours, int* count) {
    *neighbours = lteNeighbourCells;
    *count = neighbourCellCount;
}

int endsWithOK() {
    const char *okResponse = "OK\r\n";
    int len = bufferIndex;
    
    // Check if the buffer is long enough to contain "OK\r\n" and compare the last characters
    if (len >= 4 && strcmp(&buffer[len - 4], okResponse) == 0) {
        return 1;
    }
    return 0;
}

// LTE 신호 정보를 분리하여 저장하는 함수
void LTE_manager_decodePacket() {
    // 먼저 servingcell을 처리
    if (strstr(buffer, "+QENG: \"servingcell\"")) {
        // 문자열을 쉼표로 나눠서 처리
        char* token = strtok(buffer, ",");

        // 불필요한 필드 건너뜀 (state, LTE, is_tdd)
        token = strtok(NULL, ",");  // state
        token = strtok(NULL, ",");  // LTE
        token = strtok(NULL, ",");  // is_tdd

        // MCC 건너뜀
        token = strtok(NULL, ",");  // mcc

        // MNC 추출 (2자리 혹은 3자리)
        token = strtok(NULL, ",");
        lteServingCell.mnc = (uint8_t)atoi(token);

        // CID 추출 (Cell ID, 4 byte, 16진수로 변환)
        token = strtok(NULL, ",");
        lteServingCell.cid = (uint32_t)strtoul(token, NULL, 16);  // 16진수로 변환

        // PCI 추출 (2 byte)
        token = strtok(NULL, ",");
        lteServingCell.pci = (uint16_t)atoi(token);

        // EARFCN 추출 및 LTE Serving Cell의 band로 저장
        token = strtok(NULL, ",");
        lteServingCell.band = (uint16_t)atoi(token);  // EARFCN 값을 band에 저장

        // Freq Band Indicator 건너뜀 (이 정보는 사용하지 않음)
        token = strtok(NULL, ",");

        // UL bandwidth 건너뜀
        token = strtok(NULL, ",");  // ul_bandwidth

        // DL bandwidth 건너뜀
        token = strtok(NULL, ",");  // dl_bandwidth

        // TAC 건너뜀
        token = strtok(NULL, ",");  // tac

        // RSRP 추출 (1 byte)
        token = strtok(NULL, ",");
        lteServingCell.rsrp = (int8_t)atoi(token);

        // RSRQ 추출 (1 byte)
        token = strtok(NULL, ",");
        lteServingCell.rsrq = (int8_t)atoi(token);

        // 추가 필드가 있다면 필요에 따라 추출
    }

    // 인접 셀 처리
    if (strstr(buffer, "+QENG: \"neighbourcell")) {
        neighbourCellCount = 0;  // 인접 셀 카운트 초기화

        char *line = strtok(buffer, "\n");  // 줄 단위로 나누기
        while (line != NULL && neighbourCellCount < MAX_NEIGHBOUR_CELLS) {
            if (strstr(line, "+QENG: \"neighbourcell intra\"")) {
                // intra 셀 정보 추출
                sscanf(line, "+QENG: \"neighbourcell intra\",\"LTE\",%hu,%hu,%hhd,%hhd,%*d,%*d,%*d,%*d,%*d,%*d,%*d",
                    &lteNeighbourCells[neighbourCellCount].band,
                    &lteNeighbourCells[neighbourCellCount].pci,
                    &lteNeighbourCells[neighbourCellCount].rsrq,
                    &lteNeighbourCells[neighbourCellCount].rsrp);

                lteNeighbourCells[neighbourCellCount].cid = 0;  // CID는 수집할 수 없으므로 0으로 설정
                lteNeighbourCells[neighbourCellCount].mnc = lteServingCell.mnc;  // 서빙 셀의 MNC 사용
                neighbourCellCount++;
            }
            else if (strstr(line, "+QENG: \"neighbourcell inter\"")) {
                // inter 셀 정보 추출
                sscanf(line, "+QENG: \"neighbourcell inter\",\"LTE\",%hu,%hu,%hhd,%hhd,%*d,%*d,%*d,%*d,%*d",
                    &lteNeighbourCells[neighbourCellCount].band,
                    &lteNeighbourCells[neighbourCellCount].pci,
                    &lteNeighbourCells[neighbourCellCount].rsrq,
                    &lteNeighbourCells[neighbourCellCount].rsrp);

                lteNeighbourCells[neighbourCellCount].cid = 0;  // CID는 수집할 수 없으므로 0으로 설정
                lteNeighbourCells[neighbourCellCount].mnc = lteServingCell.mnc;  // 서빙 셀의 MNC 사용
                neighbourCellCount++;
            }
            line = strtok(NULL, "\n");  // 다음 줄로 이동
        }
    }
}

bool LTE_manager_readSerialBufferContains(const char* target) {
    String buffer = "";
    while (Serial2.available()) {
        char c = Serial2.read();
        buffer += c;
    }
    return buffer.indexOf(target) != -1;
}
