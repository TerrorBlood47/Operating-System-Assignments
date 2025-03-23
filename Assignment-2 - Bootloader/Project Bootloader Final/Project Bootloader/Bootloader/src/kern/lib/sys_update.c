#include <sys_update.h>
#include <flash.h>
#include <crc.h>
#include <kstdio.h>
#include <sys_init.h>
#include <cm4.h>

char updated_os[MAX_SYSTEM_ARRAY_SIZE];
// int os_size;

void reset_version(void) {
    char version = '1';
    flash_byte_write((uint8_t)version, VERSION_ADDRESS);
}

void system_update(void) {
    // reset_version();
    if (check_version() == 1) {
        fetch_update();
    }
}

int compare_strings(const char *str1, const char *str2) {
    int i = 0;

    // Loop through both strings and compare each character
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) {
            return (str1[i] - str2[i]);  // Return difference of first mismatched characters
        }
        i++;
    }

    return 0;  // Strings are identical
}

int check_version(void) {
    uint8_t version = flash_byte_read(VERSION_ADDRESS);
    // kprintf("%c\n", version);
    // kprintf("CHECK_VERSION %c\n", version);
    kprintf("CHECK_VERSION 1\n");

    char response[50];
    int i = 0;
    char c;
    do {
        kscanf("%c", &c);
        response[i++] = c;
    } while (c != '\n');

    // kprintf("%s", response);

    int ret = compare_strings(response, "UPDATE AVAILABLE");

    if (ret == 0) {
        kscanf("%c", &c);
        kprintf("Received version %c\n", c);
        // flash_byte_write((uint8_t)c, VERSION_ADDRESS);
        // version = flash_byte_read(VERSION_ADDRESS);
        // kprintf("Version %c\n", version);
        return 1;
    }

    return 0;
}

int char_array_to_int(const char *str, int n) {
    int result = 0;
    int i = 0;

    // Convert each character to an integer
    while (i < n) {
        // Ensure that the character is a digit (between '0' and '9')
        if (str[i] >= '0' && str[i] <= '9') {
            result = result * 10 + (str[i] - '0');
            // kprintf("%c\n", str[i]);
        }
        i++;
    }

    return result;
}

void fetch_update(void) {
    flash_erase(0x4, 0x6);
    ms_delay(1000);

    kprintf("Flash erase successful\n");
    kprintf("GET_UPDATE\n");

    int flag = 0;
    int file_size = 0;

    // read file size
    char c;
    char len[10];
    int j = 0;
    do {
        kscanf("%c", &c);
        if (c != '$') {
            // kprintf("%c\n", c);
            len[j++] = c;
        }
    } while (c != '$');
    // kprintf("%d\n", j);

    file_size = char_array_to_int(len, j);

    kprintf("Received file size %d Bytes\n", file_size);
    kprintf("ACK\n");

    const int CHUNK_SIZE = 500;

    CRC_Config();

    // flash_erase(0x4, 0x7);

    uint32_t start_address = OS_START_ADDRESS;

    int i = 0;

    // read file chunk by chunk
    while (file_size != 0) {
        int n = CHUNK_SIZE;
        if (file_size < CHUNK_SIZE) {
            n = file_size;
        }
        char received_data[n];
        for (int k = 0; k < n; k++) {
            kscanf("%c", &c);
            // updated_os[i++] = c;
            received_data[k] = c;
        }

        uint32_t received_crc = 0;
        for (int k = 0; k < 4; k++) {
            kscanf("%c", &c);
            // uint8_t dummy = c;
            // kprintf("%x\n", dummy);
            received_crc = (received_crc << 8) | (unsigned char)c;  // Combine 4 bytes into a 32-bit integer
        }

        // Compute the CRC32 for the received data
        // uint32_t computed_crc = crc32((uint8_t *)received_data, n);
        CRC_Reset();
        uint32_t computed_crc = CRC_Calc((uint8_t *)received_data, n);
        // kprintf("%x\n", received_crc);
        kprintf("%x\n", computed_crc);

        // Compare the computed CRC with the received CRC
        if (computed_crc == received_crc) {
            for (int k = 0; k < n; k++) {
                updated_os[i++] = received_data[k];
            }

            if (MAX_SYSTEM_ARRAY_SIZE - i < CHUNK_SIZE) {
                if (flag) {
                    flash_write((uint8_t*)updated_os, i + 1, start_address, 0);
                }
                else {
                    flash_write((uint8_t*)updated_os, i + 1, start_address, 0x1000);
                    flag = 1;
                }
                ms_delay(1000);
                start_address += (i + 1);
                i = 0;
            }

            if (file_size >= CHUNK_SIZE) {
                file_size -= CHUNK_SIZE;
            }
            else {
                file_size = 0;
            }

            kprintf("ACK\n");
            // kprintf("%d\n", file_size);
        } 
        else {
            kprintf("NACK\n");
            break;
        }

        // kprintf("ACK\n");
        // kprintf("%d\n", file_size);

    }

    if (file_size == 0 && i > 0) {
        if (flag) {
            flash_write(updated_os, i, start_address, 0);
        }
        else {
            flash_write(updated_os, i, start_address, 0x1000);
        }
        ms_delay(2000);
    }

    if (file_size == 0) {
        kprintf("File received successfully\n");
    }
    else {
        kprintf("Error while receiving file\n");
    }
}
