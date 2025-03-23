
#include <sys_init.h>
#include <cm4.h>
#include <kmain.h>
#include <stdint.h>
#include <sys_usart.h>
#include <kstdio.h>
#include <sys_rtc.h>
#include <kstring.h>
#include <sys_update.h>
#ifndef DEBUG
#define DEBUG 1
#endif

static void vector_setup(void)
{
    SCB->VTOR = BOOTLOADER_SIZE;
}

void jump_to_os(void)
{
    // *VERSION_ADDR = 10;

    typedef void (*void_fn)(void);
    uint32_t *reset_vector_entry = (uint32_t *)(OS_START_ADDRESS + 4U);
    uint32_t *reset_vector = (uint32_t *)(*reset_vector_entry);
    void_fn jump_fn = (void_fn)reset_vector;

    jump_fn();
}

/////////////////////

uint8_t psp_stack[16 * (2 ^ 10)] __attribute__((section(".psp_stack"))); // 16KB

#define PSP_STACK_END (uint32_t)(&psp_stack[16 * (2 ^ 10)])
#define PSP_STACK_SIZE (16 * (2 ^ 10))
#define PSP_STACK_START (PSP_STACK_END - PSP_STACK_SIZE)

void __set_user_mode(void)
{

    asm volatile(
        "MOV R0, %0\n"
        "MSR PSP, R0\n"
        :
        : "r"(PSP_STACK_END)
    );

    __ISB();

    uint8_t value = 0x03; // Set to unprivileged mode and PSP and FCPA=1
    asm volatile(
        "MSR CONTROL, %0\n"
        "ISB 0xF\n"
        :
        : "r"(value)
        : "memory");


    asm volatile(
        "SVC 2\n"
    );
}


void __move_to_user(void){
	uint32_t psp_stack[1024];

    kprintf("psp start : %d\n", psp_stack + 1024);
    PSP_Init(psp_stack + 1024);
   asm volatile (
		".global PSP_Init\n"
		"PSP_Init:\n"
			"msr psp, r0\n"
			"mov r0, #3\n"
			"msr control, r0\n"
			"isb\n"
	);

    asm volatile(
        "SVC 20\n"
    );

}


void kmain(void)
{
    __sys_init();

    __NVIC_SetPriority(SVCall_IRQn, 0x1); // Set SVC priority to 1 //to deal with double push of stack
    __NVIC_SetPriority(PendSV_IRQn, 0xFF); 


    __ISB();

    // __set_user_mode();

    __move_to_user();

    
    while (1)
    {
    }
}
