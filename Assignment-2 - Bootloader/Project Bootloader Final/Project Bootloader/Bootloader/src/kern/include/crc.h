#ifndef __CRC_H
#define __CRC_H

#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

void CRC_Config(void);
void CRC_Reset(void);
uint32_t CRC_Calc(uint8_t *data, uint32_t length);


#ifdef __cplusplus
}
#endif

#endif