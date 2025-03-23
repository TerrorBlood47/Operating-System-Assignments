#include <crc.h>
#include <sys_bus_matrix.h>

void CRC_Config(void) {
    RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
    CRC->CR = CRC_CR_RESET; // resets the CRC calculation unit to 0xFFFF FFFF
}

void CRC_Reset(void) {
    CRC->CR = CRC_CR_RESET; // resets the CRC calculation unit to 0xFFFF FFFF
}

uint32_t CRC_Calc(uint8_t *data, uint32_t length)
{

    for (uint32_t i = 0; i < length; i += 1)
    {
        CRC->DR = (uint32_t) data[i];
    }

    return (CRC->DR);
}
