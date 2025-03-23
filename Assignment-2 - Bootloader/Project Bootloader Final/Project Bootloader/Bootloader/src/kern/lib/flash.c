#include <flash.h>
#include <sys_init.h>
#include <cm4.h>
#include <kmain.h>
#include <sys_usart.h>
#include <kstdio.h>
#include <sys_rtc.h>
#include <kstring.h>

void flash_unlock(void){
 
    if(FLASH->CR & FLASH_CR_LOCK){
        FLASH->KEYR = FLASH_KEY1;
        FLASH->KEYR = FLASH_KEY2;
    }
}

void flash_lock(void){
    FLASH->CR |= FLASH_CR_LOCK;
}

void flash_erase(uint8_t start_sector, uint8_t end_sector) {
 
    /*
    sector 4: 0x0801 0000 - 0x0801 FFFF length= 64KB
    sector 5: 0x0802 0000 - 0x0803 FFFF length= 128KB
    sector 6: 0x0804 0000 - 0x0805 FFFF length= 128KB
    sector 7: 0x0806 0000 - 0x0807 FFFF length= 128KB
    */
 
    flash_unlock();
 
    for(uint8_t sector=start_sector; sector<=end_sector; sector++){
 
        while(FLASH->SR & FLASH_SR_BSY); // Wait for the flash to be ready
 
        FLASH->CR |= FLASH_CR_SER; // Sector erase enabled
 
        FLASH->CR &= ~(0xF << 3); // Clear the sector number 
        FLASH->CR |= sector << 3;   //select the sector to erase in hex
 
        FLASH->CR |= FLASH_CR_STRT; // start the erase operation
 
        while(FLASH->SR & FLASH_SR_BSY);
    }
 
    flash_lock();
}

int flash_erase_check(uint32_t start_address, uint32_t end_address) {

    for(int i=start_address; i<end_address; i+=4){
        if(*(uint32_t*)i != 0xFFFFFFFF){
            return 0;
        }
    }
 
    return 1;
}

void flash_byte_write(uint8_t data, uint32_t address) {
    flash_unlock();
 
    while (FLASH->SR & FLASH_SR_BSY); // Wait for the flash to be ready
 
    FLASH->CR |= FLASH_CR_PG; // Enable programming mode

    *(uint8_t *)(address) = data;

    while (FLASH->SR & FLASH_SR_BSY); // Wait for the flash to be ready

    if (*(uint8_t *)(address) != data) {
        kprintf("Verification failed at address 0x%x -- data %c \n", (address), *(uint8_t *)(address));
        flash_lock();
        return;
    }
 
    FLASH->CR &= ~FLASH_CR_PG; // Programming disabled

    flash_lock();
}

void flash_write(uint8_t *data, uint32_t length, uint32_t start_address, uint32_t start_index)
{
    // kprintf("write len : %d\n", length);
    flash_unlock();
 
    // Clear previous errors
    FLASH->SR |= (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR);
 
    while (FLASH->SR & FLASH_SR_BSY); // Wait for the flash to be ready
 
    FLASH->CR |= FLASH_CR_PG; // Enable programming mode
 
    for (uint32_t i = start_index, j = 0; i < length; i++, j++)
    {
        // Program byte (8-bit) data to flash
        *(uint8_t *)(start_address + j) = data[i];
 
        while (FLASH->SR & FLASH_SR_BSY); // Wait for the flash to be ready
 
        // Verify the written data
        if (*(uint8_t *)(start_address + j) != data[i]) {
            kprintf("index -- %x data -- %d\n", i, data[i]);
            kprintf("Verification failed at address 0x%x\n  data  ", (start_address + j), *(uint8_t *)(start_address + j));
            flash_lock();
            return;
        }
 
        // Check for errors
        if (FLASH->SR & (FLASH_SR_WRPERR | FLASH_SR_PGAERR | FLASH_SR_PGPERR | FLASH_SR_PGSERR))
        {
            kprintf("Flash write error at index: %d\n", i);
            kprintf("SR: 0x%x\n", FLASH->SR);
            kprintf("CR: 0x%x\n", FLASH->CR);
            kprintf("WRPERR: 0x%x\n", FLASH->SR & FLASH_SR_WRPERR);
            kprintf("PGAERR: 0x%x\n", FLASH->SR & FLASH_SR_PGAERR);
            kprintf("PGPERR: 0x%x\n", FLASH->SR & FLASH_SR_PGPERR);
            kprintf("PGSERR: 0x%x\n", FLASH->SR & FLASH_SR_PGSERR);
            FLASH->CR &= ~FLASH_CR_PG; // Disable programming mode
            flash_lock();
            return; // Exit on error
        }
 
        // kprintf("write index : %d -- address:0x%x\n", i, (uint8_t *)(start_address + i));
    }
 
    FLASH->CR &= ~FLASH_CR_PG; // Programming disabled
 
    flash_lock();
}

uint8_t flash_byte_read(uint32_t address) {
    uint8_t val = *(uint8_t *)(address);
    return val;
}

void flash_read(uint32_t length, uint32_t start_address)
{    
    for (uint32_t i = 0; i < length; i += 1)
    {
 
        // *(uint8_t *)(data + i) = *(uint8_t *)(start_address + i);
 
        uint8_t val = *(uint8_t *)(start_address + i);
 
        kprintf("read : %c --- index : %d --- address : 0x0%x\n", val, i, start_address + i);
 
        while (FLASH->SR & FLASH_SR_BSY);
 
 
    }
}
