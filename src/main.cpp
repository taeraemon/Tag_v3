#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
#define EEPROM_SIZE 300

#define ADDR_ssid 0
#define ADDR_pswd 50
#define ADDR_scan_toggle 100
#define ADDR_adv_interval 110
#define ADDR_transmit_power 120

#define PIN_MNT 36


#include <Arduino.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <WiFi.h>
#include <WiFiGeneric.h>
#include <WiFiClient.h>
#include <WiFiAP.h>
#include <esp_wifi.h>

#include <EEPROM.h>

#include <Wire.h>
#include <RTClib.h>

//#include <AESLib.h>



// 함수 프로토타입
//================================================================



void CommandProcessing(std::string rxValue);
void StartBLE();
void StartWiFi();
void setTime(int val);
int getTime();
void ScanAndSend();
void cleanArr(char *data_ptr);
void writeEEPROM(int addr, char *data_ptr);
void printTime();
void resetDevice();
uint8_t key[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};


// 전역변수
//================================================================



BLEServer *pServer = NULL;
BLEService *pService = NULL;
BLECharacteristic * pTxCharacteristic;
BLECharacteristic * pRxCharacteristic;

RTC_DS3231 rtc;

bool deviceConnected = false;
bool oldDeviceConnected = false;

char ssid[100] = {0,};
char pswd[100] = {0,};
char arr_scan_toggle[10] = {0,};    int scan_toggle = 1;
char arr_adv_interval[10] = {0,};  int adv_interval = 100;
char arr_transmit_power[10] = {0,}; int transmit_power = 80;





// 클래스 정의
//================================================================



class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        std::string rxValue = pCharacteristic->getValue();
        if (rxValue.length() > 0) {
            CommandProcessing(rxValue);
        }
    }
};



// 함수 정의
//================================================================



void CommandProcessing(std::string rxValue){
    Serial.print("Received Value: ");
    for (int i = 0; i < rxValue.length(); i++){
        Serial.print(rxValue[i]);
    }
    Serial.println();

    char command[100] = {0,};
    strcpy(command, &rxValue[0]);


    cleanArr(command);
    // USER COMMAND PROCESSING
    if(command[0] == '1'){          // SCN  // wifi SCAN 동작
        if(command[1] == '0'){
            scan_toggle = 0;
            writeEEPROM(ADDR_scan_toggle, &command[1]);
        }
        else if(command[1] == '1'){
            scan_toggle = 1;
            writeEEPROM(ADDR_scan_toggle, &command[1]);
        }
    }

    else if(command[0] == '2'){     // SID
        strcpy(ssid, &command[2]);
        
        if(command[1] == '1'){          // EEPROM에도 변경된 ssid를 저장
            writeEEPROM(ADDR_ssid, ssid);
        }

        StartWiFi();
        esp_ble_gap_set_device_name(ssid);
    }

    else if(command[0] == '3'){     // 기타 등등 설정
        int val = atoi(&command[2]);
        Serial.println(val);

        if(rxValue[1] == '1'){          // broadcasting, advertise 주기 설정
            Serial.println("31 input");
            
            adv_interval = val;

            int val_map = (val, 0, 9, 100, 50000);
            wifi_ap_config_t config_w;
            config_w.beacon_interval = val_map;

            esp_ble_adv_params_t config_b;
            config_b.adv_int_min = val_map;
        }
        if(rxValue[1] == '2'){          // wifi maximum transmit power 설정
            Serial.println("32 input");
            esp_wifi_set_max_tx_power(val); // 0.25dBm per, 8 ~ 80 (2dBm ~ 20dBm)
            
        }
    }

    else if(command[0] == '4'){     // BAT      배터리 상태 모니터링
        int volt_sum = 0, volt_avg = 0;
        for(int i=0;i<10;i++){
            volt_sum = volt_sum + (analogReadMilliVolts(PIN_MNT) * 2);
        }
        volt_avg = volt_sum / 10;
        
        Serial.print("Current Battery voltage : ");
        Serial.println(volt_avg);

        char buf[50] = {0,};
        int volt_map = map(volt_avg, 3000, 4200, 0, 100);
        sprintf(buf, "Battery : %d%%\n", volt_map);
        
        pTxCharacteristic->setValue(buf);
        pTxCharacteristic->notify();
    }

    else if(command[0] == '5'){     // 시간 동기화
        Serial.println("Time set");
        
        int val = atoi(&command[1]);
        setTime(val);

        printTime();
    }

    else if(command[0] == '6'){
        Serial.println("reset device");
        resetDevice();
    }
}








void StartBLE(){
    BLEDevice::init(ssid);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    pService = pServer->createService(SERVICE_UUID);

    pTxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_TX, BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());
    
    pRxCharacteristic = pService->createCharacteristic(CHARACTERISTIC_UUID_RX, BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    pService->start();
    pServer->getAdvertising()->start();
}

void StartWiFi(){
    WiFi.mode(WIFI_AP);
    WiFi.disconnect();
    delay(100);
    WiFi.softAP(ssid, pswd);
}

void setTime(int val){
    rtc.adjust(DateTime(val));
}

int getTime(){
    DateTime now = rtc.now();
    return (int)(now.unixtime());
}

void writeEEPROM(int addr, char *data_ptr){
    Serial.println("writeEEPROM Start");
    EEPROM.write(addr, strlen(data_ptr));
    for(int i=0;i<strlen(data_ptr);i++){
        EEPROM.write(addr+1+i, data_ptr[i]);
    }
    Serial.println("in WriteEEPROM");
    Serial.println(strlen(data_ptr));
    EEPROM.commit();
    Serial.println(data_ptr);
    Serial.println("writeEEPROM End");
}

void readEEPROM(int addr, char *data_ptr){
    for(int i=0;i<byte(EEPROM.read(addr));i++){
        data_ptr[i] = (char)byte(EEPROM.read(addr+1+i));
    }
}

void resetDevice(){
    // EEPROM Write
    // ================================
    // WiFi, BLE ssid
    EEPROM.write(0, 7);
    EEPROM.write(1, 'd');
    EEPROM.write(2, 'e');
    EEPROM.write(3, 'f');
    EEPROM.write(4, 'a');
    EEPROM.write(5, 'u');
    EEPROM.write(6, 'l');
    EEPROM.write(7, 't');

    // WiFi, BLE pswd
    EEPROM.write(50, 8);
    EEPROM.write(51, '0');
    EEPROM.write(52, '0');
    EEPROM.write(53, '0');
    EEPROM.write(54, '0');
    EEPROM.write(55, '0');
    EEPROM.write(56, '0');
    EEPROM.write(57, '0');
    EEPROM.write(58, '0');

    // WiFi scan set (default : on)
    EEPROM.write(100, 1);
    EEPROM.write(101, '1');

    // adv scan interval (default : 0(range : 0~9))
    EEPROM.write(110, 1);
    EEPROM.write(111, '0');

    // WiFi broadcast transmit power (default : max(80))
    EEPROM.write(120, 2);
    EEPROM.write(121, '8');
    EEPROM.write(122, '0');
    
    EEPROM.commit();
}

void cleanArr(char *data_ptr){
    for(int i=0;i<strlen(data_ptr);i++){
        if(data_ptr[i] == '\r' || data_ptr[i] == '\n'){
            data_ptr[i] = 0;
        }
    }
}

void printTime(){
    DateTime now = rtc.now();
    Serial.print(now.year(), DEC); Serial.print('/');
    Serial.print(now.month(), DEC); Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    
    Serial.print(now.hour(), DEC); Serial.print(':');
    Serial.print(now.minute(), DEC); Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println("  (UTC)");

    now = rtc.now() + 9*3600;
    Serial.print(now.year(), DEC); Serial.print('/');
    Serial.print(now.month(), DEC); Serial.print('/');
    Serial.print(now.day(), DEC);
    Serial.print(" ");
    
    Serial.print(now.hour(), DEC); Serial.print(':');
    Serial.print(now.minute(), DEC); Serial.print(':');
    Serial.print(now.second(), DEC);
    Serial.println("  (KST)");
}

void ScanAndSend(){
    digitalWrite(LED_BUILTIN, HIGH);
    int n = WiFi.scanNetworks();
    digitalWrite(LED_BUILTIN, LOW);
    
    char buf[50] = {0,};
    String str = String(getTime()) + "\n";
    str.toCharArray(buf, 50);
    pTxCharacteristic->setValue(buf);
    pTxCharacteristic->notify();
    if (n != 0) {
        for(int i=0;i<n;i++){
            char buf[200] = {0,};
            String str = String(WiFi.SSID(i)) + "#" + WiFi.BSSIDstr(i) + "#" + String(WiFi.RSSI(i)) + "\n";
            str.toCharArray(buf, 200);
            pTxCharacteristic->setValue(buf);
            pTxCharacteristic->notify();
        }
    }
}

/*
void Encrypto{
    char user_id_crypto[50] = "tag_private";
    user_id.toCharArray(user_id_crypto, 50);
    aes256_enc_single(key, user_id_crypto);
    Serial.print("encrypt: ");
    Serial.println(user_id_crypto);
}

void Decrypto{
    char user_id_crypto[50];
    aes256_dec_single(key, user_id_crypto);
    Serial.print("decrypt: ");
    Serial.println(user_id_crypto);
}
*/

//================================================================
//================================================================
//================================================================



void setup() {
    Serial.begin(9600);
    pinMode(LED_BUILTIN, OUTPUT);

    while(1){
        if (EEPROM.begin(EEPROM_SIZE)){
            Serial.println("EEPROM Connection Success.");
            break;
        }
        else{
            Serial.println("EEPROM Connection failed.");
            delay(1000000);
        }
    }

    readEEPROM(ADDR_ssid, ssid);
    readEEPROM(ADDR_pswd, pswd);
    readEEPROM(ADDR_scan_toggle, arr_scan_toggle);
    readEEPROM(ADDR_adv_interval, arr_adv_interval);
    readEEPROM(ADDR_transmit_power, arr_transmit_power);
    scan_toggle = atoi(arr_scan_toggle);
    adv_interval = atoi(arr_adv_interval);
    transmit_power = atoi(arr_transmit_power);
    
    Serial.println(ssid);
    Serial.println(pswd);
    Serial.println(scan_toggle);
    Serial.println(adv_interval);
    Serial.println(transmit_power);
    Serial.println();
    
    StartBLE();
    StartWiFi();

    rtc.begin();
    printTime();

    Serial.println("Setup done");
    analogReadResolution(12);
}

//================================================================

void loop() {
    if (deviceConnected) {
        if(scan_toggle){
            ScanAndSend();
            delay(100);
        }
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}
