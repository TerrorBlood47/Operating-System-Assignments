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

#ifndef __KMAIN_H
#define __KMAIN_H
#ifdef __cplusplus
extern "C" {
#endif

#define MAX_TASKS   5

/* some stack memory calculations */
#define SIZE_TASK_STACK          1024U
#define SIZE_SCHED_STACK         1024U

#define TICK_HZ 1000U

#define HSI_CLOCK         		16000000U
#define SYSTICK_TIM_CLK   		HSI_CLOCK


#define DUMMY_XPSR  0x01000000U

#define TASK_READY_STATE  0x00
#define TASK_BLOCKED_STATE  0XFF

#define INTERRUPT_DISABLE()  do{__asm volatile ("MOV R0,#0x1"); asm volatile("MSR PRIMASK,R0"); } while(0)

#define INTERRUPT_ENABLE()  do{__asm volatile ("MOV R0,#0x0"); asm volatile("MSR PRIMASK,R0"); } while(0)

//by FAIAK
#define RUNNING 0
#define WAITING 1
#define READY 2
#define KILLED 3
#define TERMINATED 4


extern volatile uint16_t  TASK_ID;
const static uint32_t MAGIC_NUMBER = 0xFECABAA0;
const static uint32_t DIGITAL_SIGNATURE = 0x00000001; 

#define MAX_QUEUE_SIZE_P  5


extern volatile uint32_t QUEUE_SIZE_P ;
extern volatile uint32_t CURR_TASK_P ;

extern volatile TCB_TypeDef READY_QUEUE[MAX_QUEUE_SIZE_P];

void create_tcb(TCB_TypeDef *tcb, void(*func_ptr)(void), uint32_t* stack_start);




#define O_RDONLY 0
#define O_WRONLY 1
#define O_APPEND 2

/* Constants for read/write/etc: special file handles */
#define STDIN_FILENO  0      /* Standard input */
#define STDOUT_FILENO 1      /* Standard output */
#define STDERR_FILENO 2      /* Standard error */

#define MAX_FILES 64

typedef struct file_entry {
    char name[32];     // File name
    uint32_t *address; // Memory-mapped file location
    uint32_t size;     // File size in bytes
    uint8_t mode;      // File mode
} file_entry_t;


extern volatile file_entry_t file_list[MAX_FILES];
extern volatile uint32_t file_count;

int find_file(char *filename);


#ifdef __cplusplus
}
#endif
#endif /* KMAIN_H */

