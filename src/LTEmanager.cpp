#include <Arduino.h>
#include "LTEmanager.h"
#include "DeviceConfig.h"

char buffer[512];  // Serial buffer
int bufferIndex = 0;

void LTEmanager_sendATCommand(const char* command) {
    Serial2.write(command);  // Send the given command to the modem
}

void LTEmanager_readSerialBuffer() {
    while (Serial2.available()) {
        char c = Serial2.read();

        if (bufferIndex < sizeof(buffer) - 1) {  // Check buffer capacity
            buffer[bufferIndex++] = c;  // Add char to buffer
            buffer[bufferIndex] = '\0'; // Null terminate string
        } else {
            // Buffer overflow protection: handle the case where buffer is full
            Serial.println("Warning: Buffer Overflow");
            bufferIndex = 0;  // Reset buffer to avoid overflow
            buffer[0] = '\0';
        }

        // Check if buffer ends with "OK\r\n"
        if (endsWithOK()) {
            LTEmanager_decodePacket();
            bufferIndex = 0;  // Clear buffer after decoding
            buffer[0] = '\0';
        }
    }
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

void LTEmanager_decodePacket() {
    // Print the decoded packet to the serial monitor with the current time in milliseconds
    Serial.print("Decoded Packet at ");
    Serial.print(millis());
    Serial.println(" ms: ");
    Serial.println(buffer);  // Print the entire buffer
}



void connectTCP() {
    // TCP 연결 요청 (소켓 번호 0, TCP 사용, SERVER_IP 및 SERVER_PORT 대체 필요)
    Serial2.println("AT+QIOPEN=1,0,\"TCP\",\"<SERVER_IP>\",<SERVER_PORT>,0,1");
    delay(2000);  // 연결 대기

    // 연결 상태 확인
    Serial2.println("AT+QISTATE=0,0");  // 소켓 0의 상태 확인
    delay(1000);  // 상태 응답 대기

    // 시리얼 버퍼에서 응답 확인
    if (LTEmanager_readSerialBufferContains("CONNECT OK")) {
        Serial.println("TCP 연결 성공, 데이터 전송 중...");
        sendBinaryPacket();
    } else {
        Serial.println("TCP 연결 비정상");
    }
}

void sendBinaryPacket() {
    // 바이너리 데이터 전송 준비 (패킷 길이를 정해야 함)
    const char binaryData[] = { 0x01, 0x02, 0x03, 0x04 };  // 예시 바이너리 데이터
    int packetLength = sizeof(binaryData);

    // 데이터 전송 명령어 (소켓 번호 0)
    Serial2.print("AT+QISEND=0,");
    Serial2.println(packetLength);
    delay(500);  // 전송 준비 대기

    // 실제 바이너리 데이터 전송
    for (int i = 0; i < packetLength; i++) {
        Serial2.write(binaryData[i]);
    }

    // 데이터 전송 완료를 위해 Ctrl+Z (0x1A) 전송
    Serial2.write(0x1A);

    // 확인 메시지
    Serial.println("바이너리 데이터 전송 완료");
}

bool LTEmanager_readSerialBufferContains(const char* target) {
    String buffer = "";
    while (Serial2.available()) {
        char c = Serial2.read();
        buffer += c;
    }
    return buffer.indexOf(target) != -1;
}
