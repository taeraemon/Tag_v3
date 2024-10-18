import socket
import struct

# 서버 설정
HOST = '0.0.0.0'  # 모든 인터페이스에서 연결을 허용
PORT = 55552      # 서버가 수신할 포트

def decode_lte_cell(packet, packetLength):
    # LTE Cell 정보 (11바이트)
    cid = struct.unpack('>I', packet[packetLength:packetLength+4])[0]
    packetLength += 4
    pci = struct.unpack('>H', packet[packetLength:packetLength+2])[0]
    packetLength += 2
    band = struct.unpack('>H', packet[packetLength:packetLength+2])[0]
    packetLength += 2
    mnc = packet[packetLength]
    packetLength += 1
    rsrp = struct.unpack('>b', packet[packetLength:packetLength+1])[0]
    packetLength += 1
    rsrq = struct.unpack('>b', packet[packetLength:packetLength+1])[0]
    packetLength += 1
    
    return {
        'CID': cid,
        'PCI': pci,
        'Band': band,
        'MNC': mnc,
        'RSRP': rsrp,
        'RSRQ': rsrq
    }, packetLength

def decode_packet(packet):
    packetLength = 0
    
    # 1. 수집 시간 (UnixTime, 4바이트)
    unixTime = struct.unpack('>I', packet[packetLength:packetLength+4])[0]
    packetLength += 4
    print(f"Unix Time: {unixTime}")
    
    # 2. 총 바이트 수 (2바이트)
    totalBytes = struct.unpack('>H', packet[packetLength:packetLength+2])[0]
    packetLength += 2
    print(f"Total Bytes: {totalBytes}")
    
    # 3. LTE 신호 수 (1바이트)
    lteSignalCount = packet[packetLength]
    packetLength += 1
    print(f"LTE Signal Count: {lteSignalCount}")
    
    # 4. LTE Serving Cell 정보 (11바이트)
    servingCell, packetLength = decode_lte_cell(packet, packetLength)
    print(f"Serving Cell: {servingCell}")
    
    # 5. LTE 인접 셀 정보 (신호당 11바이트)
    for i in range(lteSignalCount - 1):
        neighbourCell, packetLength = decode_lte_cell(packet, packetLength)
        print(f"Neighbour Cell {i+1}: {neighbourCell}")
    
    # 6. WiFi 신호 수 (1바이트)
    wifiSignalCount = packet[packetLength]
    packetLength += 1
    print(f"WiFi Signal Count: {wifiSignalCount}")
    
    # 7. WiFi 신호 정보 (신호당 7바이트)
    for i in range(wifiSignalCount):
        mac = packet[packetLength:packetLength+6]
        packetLength += 6
        rssi = struct.unpack('>b', packet[packetLength:packetLength+1])[0]
        packetLength += 1
        
        mac_str = ':'.join(f'{b:02X}' for b in mac)
        print(f"WiFi {i+1} - MAC: {mac_str}, RSSI: {rssi}")

def start_server():
    # 소켓 생성
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen(5)  # 최대 5개의 연결 대기 가능
        
        print(f"Server started on port {PORT}. Waiting for connection...")
        
        while True:  # 서버를 계속 실행
            conn, addr = s.accept()  # 클라이언트 연결 수락
            print(f"Connected by {addr}")
            
            with conn:
                while True:
                    # 데이터를 1024바이트 수신
                    data = conn.recv(1024)
                    if not data:
                        break
                    
                    print("Packet received. Decoding...")
                    decode_packet(data)  # 수신한 패킷 디코딩
                print(f"Connection closed from {addr}")

if __name__ == "__main__":
    start_server()
