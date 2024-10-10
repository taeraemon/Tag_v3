#ifndef LTE_MANAGER_H
#define LTE_MANAGER_H

#include <Arduino.h>

struct LTEInfo {
    uint32_t cid;
    uint16_t pci;
    uint8_t band;
    uint16_t mnc;
    int8_t rsrp;
    int8_t rsrq;
};

struct LTENeighbourCellInfo {
    uint16_t pci;
    uint32_t cid;
    int8_t rsrp;
    int8_t rsrq;
    bool isIntra;
};

void LTE_manager_sendATCommand(const char* command);
void LTE_manager_readSerialBuffer();
void LTE_manager_decodePacket();
void clearLTEData();
int endsWithOK();
LTEInfo getLTEData();
void getLTENeighbourCells(LTENeighbourCellInfo** neighbours, int* count);
bool LTE_manager_readSerialBufferContains(const char* target);

#endif // LTE_MANAGER_H
