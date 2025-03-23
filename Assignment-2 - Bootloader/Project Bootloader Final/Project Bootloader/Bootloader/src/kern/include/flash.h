#ifndef __FLASH_H
#define __FLASH_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_KEY1 0x45670123
#define FLASH_KEY2 0xCDEF89AB

void flash_unlock(void);
void flash_lock(void);
void flash_erase(uint8_t start_sector, uint8_t end_sector);
int flash_erase_check(uint32_t start_address, uint32_t end_address);
void flash_byte_write(uint8_t data, uint32_t address);
void flash_write(uint8_t *data, uint32_t length, uint32_t start_address, uint32_t start_index);
void flash_read(uint32_t length, uint32_t start_address);
uint8_t flash_byte_read(uint32_t address);

#ifdef __cplusplus
}
#endif

#endif