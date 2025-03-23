

#include <sys_init.h>
#include <cm4.h>
#include <kmain.h>
#include <stdint.h>
#include <sys_usart.h>
#include <kstdio.h>
#include <sys_rtc.h>
#include <kstring.h>
#ifndef DEBUG
#define DEBUG 1
#endif

// #define BOOTLOADER_SIZE         (0x00010000U) //64 KB

// static void vector_setup(void){
//     SCB->VTOR = BOOTLOADER_SIZE; 
// }

void kmain(void)
{
    // vector_setup();
    __sys_init();
    while (1)
    {
      
    }
}
