#ifndef __SYS_UPDATE_H
#define __SYS_UPDATE_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_BASE_ADDRESS          (0x08000000U)
#define VERSION_ADDRESS             (0x08060000U)
#define BOOTLOADER_SIZE             (0x00010000U) // 64 KB
#define OS_START_ADDRESS            (FLASH_BASE_ADDRESS + BOOTLOADER_SIZE) // 0X08010000
#define MAX_SYSTEM_ARRAY_SIZE       100005

void system_update(void);
int compare_strings(const char *str1, const char *str2);
int check_version(void);
int char_array_to_int(const char *str, int n);
void fetch_update(void);

#ifdef __cplusplus
}
#endif

#endif