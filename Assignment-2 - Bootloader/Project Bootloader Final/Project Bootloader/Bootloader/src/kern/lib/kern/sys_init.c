/*
 * Copyright (c) 2022 
 * Computer Science and Engineering, University of Dhaka
 * Credit: CSE Batch 25 (starter) and Prof. Mosaddek Tushar
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE UNIVERSITY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE UNIVERSITY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys_init.h>
#include <cm4.h>
#include <sys_clock.h>
#include <sys_usart.h>
#include <serial_lin.h>
#include <sys_gpio.h>
#include <kstdio.h>
#include <debug.h>
#include <timer.h>
#include <UsartRingBuffer.h>
#include <system_config.h>
#include <mcu_info.h>
#include <sys_rtc.h>
#ifndef DEBUG
#define DEBUG 1
#endif
extern UART_HandleTypeDef huart6;

void display_status(void);
int check_version(void);
void system_update(void);

// char updated_os[100005];
// int os_size;

void __sys_init(void)
{
	__init_sys_clock(); //configure system clock 180 MHz
	__ISB();	
	__enable_fpu(); //enable FPU single precision floating point unit
	__ISB();
	NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
	__SysTick_init(1000);	//enable systick for 1ms
	//SYS_RTC_init();
	SerialLin2_init(__CONSOLE,0);
	SerialLin6_init(&huart6,0);
	Ringbuf_init(__CONSOLE);
	Ringbuf_init(&huart6);
	ConfigTimer2ForSystem();
	__ISB();
	#ifdef DEBUG
	// kprintf("\n************************************\r\n");
	// kprintf("Booting Machine Intelligence System 1.0 .....\r\n");
	// kprintf("Copyright (c) 2024, Prof. Mosaddek Tushar, CSE, DU\r\n");
	// kprintf("CPUID %x\n", SCB->CPUID);
	// kprintf("OS Version: 2024.1.0.0\n");
	// kprintf("Time Elapse %d ms\n",__getTime());
	// kprintf("*************************************\r\n");
	// kprintf("# ");
	// show_system_info();
	display_status();
    // if (check_version() == 1) {
    //     system_update();
    // }
	#endif
}

void __sys_disable(void)
{
 
	// DISABLE ALL PERIPHERALS
	DisableUart(&huart2);
	DisableUart(&huart6);
 
	// DISABLE TIMER2
	DisableTimer2();
 
	// DISABLE ALL INTERRUPTS
	NVIC_DisableIRQ(USART2_IRQn);
	NVIC_DisableIRQ(USART6_IRQn);
	NVIC_DisableIRQ(TIM2_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);
	NVIC_DisableIRQ(FPU_IRQn);
 
	ms_delay(5000);
}

/*
* Do not remove it is for debug purpose
*/

void SYS_ROUTINE(void)
{
	__debugRamUsage();
}


void display_status(void)
{
    kprintf("Bootloader is running...\n");
}

// int compare_strings(const char *str1, const char *str2) {
//     int i = 0;

//     // Loop through both strings and compare each character
//     while (str1[i] != '\0' && str2[i] != '\0') {
//         if (str1[i] != str2[i]) {
//             return (str1[i] - str2[i]);  // Return difference of first mismatched characters
//         }
//         i++;
//     }

//     return 0;  // Strings are identical
// }

// int check_version(void) {
//     kprintf("CHECK_VERSION 1.0\n");

//     char response[50];
//     int i = 0;
//     char c = "";
//     do {
//         kscanf("%c", &c);
//         response[i++] = c;
//     } while (c != '\n');

//     kprintf("%s", response);

//     int ret = compare_strings(response, "UPDATE AVAILABLE");

//     if (ret == 0) {
//         return 1;
//     }

//     return 0;
// }

// int char_array_to_int(const char *str, int n) {
//     int result = 0;
//     int i = 0;

//     // Convert each character to an integer
//     while (i < n) {
//         // Ensure that the character is a digit (between '0' and '9')
//         if (str[i] >= '0' && str[i] <= '9') {
//             result = result * 10 + (str[i] - '0');
//             // kprintf("%c\n", str[i]);
//         }
//         i++;
//     }

//     return result;
// }

// // void CRC_Config(void) {
// //     RCC->AHB1ENR |= RCC_AHB1ENR_CRCEN;
// //     CRC->CR = CRC_CR_RESET; // resets the CRC calculation unit to 0xFFFF FFFF
// // }

// // void CRC_Reset(void) {
// //     CRC->CR = CRC_CR_RESET; // resets the CRC calculation unit to 0xFFFF FFFF
// // }

// // uint32_t CRC_Calc(uint8_t *data, uint32_t length)
// // {

// //     for (uint32_t i = 0; i < length; i += 1)
// //     {
// //         CRC->DR = (uint32_t) data[i];
// //     }

// //     return (CRC->DR);
// // }


// void system_update(void) {
//     kprintf("GET_UPDATE\n");

//     int i = 0;
//     int file_size = 0;

//     // read file size
//     char c = "";
//     char len[10];
//     int j = 0;
//     do {
//         kscanf("%c", &c);
//         if (c != '$') {
//             // kprintf("%c\n", c);
//             len[j++] = c;
//         }
//     } while (c != '$');
//     // kprintf("%d\n", j);

//     file_size = char_array_to_int(len, j);
//     os_size = file_size;

//     // kprintf("%d\n", file_size);
//     kprintf("ACK\n");

//     const int CHUNK_SIZE = 500;

//     CRC_Config();

//     // read file chunk by chunk
//     while (file_size != 0) {
//         c = "";
//         int n = CHUNK_SIZE;
//         if (file_size < CHUNK_SIZE) {
//             n = file_size;
//         }
//         char received_data[n];
//         for (int k = 0; k < n; k++) {
//             kscanf("%c", &c);
//             // updated_os[i++] = c;
//             received_data[k] = c;
//         }

//         uint32_t received_crc = 0;
//         for (int k = 0; k < 4; k++) {
//             kscanf("%c", &c);
//             // uint8_t dummy = c;
//             // kprintf("%x\n", dummy);
//             received_crc = (received_crc << 8) | (unsigned char)c;  // Combine 4 bytes into a 32-bit integer
//         }

//         // Compute the CRC32 for the received data
//         // uint32_t computed_crc = crc32((uint8_t *)received_data, n);
//         CRC_Reset();
//         uint32_t computed_crc = CRC_Calc((uint8_t *)received_data, n);
//         // kprintf("%x\n", received_crc);
//         // kprintf("%x\n", computed_crc);

//         // Compare the computed CRC with the received CRC
//         if (computed_crc == received_crc) {
//             kprintf("ACK\n");
//             for (int k = 0; k < n; k++) {
//                 updated_os[i++] = received_data[k];
//             }

//             if (file_size >= CHUNK_SIZE) {
//             file_size -= CHUNK_SIZE;
//             }
//             else {
//                 file_size = 0;
//             }
//         } else {
//             kprintf("NACK\n");
//             break;
//         }

//         // kprintf("ACK\n");
//         // kprintf("%d\n", file_size);

//     }

//     if (file_size == 0) {
//         kprintf("File received successfully\n");
//     }
//     else {
//         kprintf("Error while receiving file\n");
//     }

//     kprintf("File size %d Bytes\n", os_size);
//     kprintf("%d\n", i);
// }

// char* get_updated_os(void) {
//     return updated_os;
// }

// int get_size(void) {
//     return os_size;
// }
