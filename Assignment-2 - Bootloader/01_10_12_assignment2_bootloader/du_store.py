import math
import os
import serial
from colorama import Fore, Style

def crc32(data, crc = 0xFFFFFFFF) -> int:
    # CRC-32 (Ethernet) polynomial 0x04C11DB7
    polynomial = 0x04C11DB7

    crc = crc ^ data

    for i in range(32):
        if (crc & 0x80000000) != 0:
            crc = (crc << 1) ^ polynomial
            
        else:
            crc = crc << 1
    
    return crc & 0xFFFFFFFF


def crc_calc(data, crc = 0xFFFFFFFF) -> int:
    for byte in data:
        crc = crc32(byte, crc)
    return crc


CHUNK_SIZE = 500 # 32 bytes data
FILE_PATH = 'testcases/duos4'

serial_instance = serial.Serial(port="/dev/ttyACM0", baudrate=115200)

while True:
    value = serial_instance.readline().decode('utf-8').strip()
    print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + value)
    
    cmd = value.split()
    # print(cmd)
    
    if cmd[0] == "CHECK_VERSION":
        with open('version.txt', 'r') as file:
            latest_version = int(file.read())
            
        current_version = float(cmd[1])
        if math.isclose(current_version, latest_version, rel_tol=1e-9):
            response = "NO UPDATES AVAILABLE\n".encode('utf-8')
            print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f'No update available')
        else:
            response = f"UPDATE AVAILABLE\n{latest_version}".encode('utf-8')
            print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f'Update available. Latest available version {latest_version}')
        serial_instance.write(response)
        
        
    elif cmd[0] == "GET_UPDATE":
        # First send the total file size in bytes
        file_size = os.path.getsize(FILE_PATH)
        print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"File size {file_size} Bytes")
        packet = str(file_size).encode('utf-8') + b'$'
        
        serial_instance.write(packet)
        
        response = serial_instance.readline().decode('utf-8').strip()
        print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + response)
        
        ack = serial_instance.readline().decode('utf-8').strip()
        print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + ack)
        
        if ack == "ACK":
            print("File size sent successfully")
            
            with open(FILE_PATH, 'rb') as file:
                current_chunk_number = 1
                total_chunk_number = int(math.ceil(file_size / CHUNK_SIZE))
                
                while file_size:
                    chunk = file.read(CHUNK_SIZE)
                    # packet = chunk + b'$'
                    # packet = chunk
                    
                    # Calculate CRC32
                    # crc = binascii.crc32(chunk) & 0xFFFFFFFF  # Ensure CRC is unsigned 32-bit
                    
                    # Convert CRC to 4 bytes (big-endian)
                    # crc_bytes = crc.to_bytes(4, byteorder='big')
                    crc = crc_calc(chunk)
                    crc_bytes = crc.to_bytes(4, byteorder='big')
                    print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"CRC {format(crc, 'X')}")
                    # print(f"CRC_bytes : {crc_bytes}")
                    
                    
                    # Append CRC to the data
                    packet = chunk + crc_bytes
                    
                    serial_instance.write(packet)
                    
                    print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f'Sent chunk number {current_chunk_number}')
                    
                    response = serial_instance.readline().decode('utf-8').strip()
                    print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + f"CRC {response}")
                    
                    ack = serial_instance.readline().decode('utf-8').strip()
                    print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + ack)
                    
                    if ack == "ACK":
                        print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"{current_chunk_number} / {total_chunk_number} chunks are sent succesfully")
                        current_chunk_number += 1
                        
                        if file_size >= CHUNK_SIZE:
                            file_size -= CHUNK_SIZE
                        else:
                            file_size = 0
                        print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"Remaining file size: {file_size}")
                    elif ack == "NACK":
                        while ack != 'ACK':
                            print( f"Error while sending chunk number {current_chunk_number}")
                            serial_instance.write(packet)
                    
                            print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f'Resent chunk number {current_chunk_number}')
                            
                            response = serial_instance.readline().decode('utf-8').strip()
                            print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + f"CRC {response}")
                            
                            ack = serial_instance.readline().decode('utf-8').strip()
                            print(Fore.LIGHTMAGENTA_EX + "STM32 : " + Style.RESET_ALL + ack)
                        
                        print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"{current_chunk_number} / {total_chunk_number} chunks are sent succesfully")
                        current_chunk_number += 1
                        
                        if file_size >= CHUNK_SIZE:
                            file_size -= CHUNK_SIZE
                        else:
                            file_size = 0
                        print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + f"Remaining file size: {file_size}")
                        
            
            if file_size == 0:
                print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + "File sent successfully")
                    
        elif ack == "NACK":
            print(Fore.LIGHTGREEN_EX + "Server: " + Style.RESET_ALL + "Error while sending file size")
                
