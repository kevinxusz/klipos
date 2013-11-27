//
// gcc startup for LPC core
// Kamal Mostafa <kamal@whence.com>
//
// License: mixed
// - simple crt0 and statically allocated stack (Kamal Mostafa; Public Domain)
// - Note: empty boilerplate ISR routines from cr_startup_lpc8xx.c
//



//*****************************************************************************
//
// Allocated stack space
//
//*****************************************************************************
#ifndef STACKSIZE
#       define STACKSIZE 64
#endif

static unsigned int StackMem[STACKSIZE];
#define _pStackTop ((void *)((unsigned int)StackMem + sizeof(StackMem)))

//*****************************************************************************

#define WEAK __attribute__ ((weak))
#define ALIAS(f) __attribute__ ((weak, alias (#f)))

//*****************************************************************************
//
// Forward declaration of the default handlers. These are aliased.
// When the application defines a handler (with the same name), this will
// automatically take precedence over these weak definitions
//
//*****************************************************************************

WEAK void Reset_Handler(void);
WEAK void NMI_Handler(void);
WEAK void HardFault_Handler(void);
WEAK void MemManage_Handler(void);
WEAK void BusFault_Handler(void);
WEAK void UsageFault_Handler(void);
WEAK void SVC_Handler(void);
WEAK void DebugMon_Handler(void);
WEAK void PendSV_Handler(void);
WEAK void SysTick_Handler(void);
WEAK void IntDefaultHandler(void);

//*****************************************************************************
//
// Forward declaration of the specific IRQ handlers. These are aliased
// to the IntDefaultHandler, which is a 'forever' loop. When the application
// defines a handler (with the same name), this will automatically take
// precedence over these weak definitions
//
//*****************************************************************************

#include "lpc13xx/isr-vector.h"


//*****************************************************************************
//
// The vector table.
// This relies on the linker script to place at correct location in memory.
//
//*****************************************************************************
extern void (* const g_pfnVectors[])(void);

__attribute__ ((section(".isr_vector")))
void (* const g_pfnVectors[])(void) = {

    // Core Level - CM3
    
    _pStackTop,                  /* Top of Stack                 */
    Reset_Handler,               /* Reset Handler                */
    NMI_Handler,                 /* NMI Handler                  */
    HardFault_Handler,           /* Hard Fault Handler           */
    MemManage_Handler,           /* MPU Fault Handler            */
    BusFault_Handler,            /* Bus Fault Handler            */
    UsageFault_Handler,          /* Usage Fault Handler          */
    0,                           /* Checksum, see LPC1xxx manual */
    0,                           /* Reserved                     */
    0,                           /* Reserved                     */
    0,                           /* Reserved                     */
    SVC_Handler,                 /* SVCall Handler               */
    DebugMon_Handler,            /* Debug Monitor Handler        */
    0,                           /* Reserved                     */
    PendSV_Handler,              /* PendSV Handler               */
    SysTick_Handler,             /* SysTick Handler              */
    
    // Chip Level 
    
#ifdef MCU_IS_LPC1311
#       include "lpc13xx/isr-vector-lpc1311.c"
#endif
    
#ifdef MCU_IS_LPC1315
#       include "lpc13xx/isr-vector-lpc1315.c"
#endif
            
            
}; /* End of g_pfnVectors */


//*****************************************************************************
// Reset entry point for your code.
// Sets up a simple C runtime environment.
//*****************************************************************************

extern unsigned int _etext;
extern unsigned int _data;
extern unsigned int _edata;
extern unsigned int _bss;
extern unsigned int _ebss;

// Simple gcc-compatible C runtime init
static inline void crt0(void)
{
    unsigned int *src, *dest, *dend;
    
    // copy the data section
    src  = (unsigned int *)(&_etext);
    dest = (unsigned int *)(&_data);
    dend = (unsigned int *)(&_edata);
    while (dest < dend)
	*(dest++) = *(src++);
    
    // blank the bss section
    dest = (unsigned int *)(&_bss);
    dend = (unsigned int *)(&_ebss);
    while (dest < dend)
	*(dest++) = 0;
}

extern void initLowLevelCpu(void);
extern int main(void);

__attribute__ ((section(".after_vectors")))
void Reset_Handler(void)
{
    crt0();
    initLowLevelCpu();
    main();
    
    while (1) ;	// hang if main returns
}

//*****************************************************************************
// Default exception handlers. Override the ones here by defining your own
// handler routines in your application code.
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void NMI_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void HardFault_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SVCall_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void PendSV_Handler(void)
{
    while(1)
    {
    }
}
__attribute__ ((section(".after_vectors")))
void SysTick_Handler(void)
{
    while(1)
    {
    }
}

//*****************************************************************************
//
// Processor ends up here if an unexpected interrupt occurs or a specific
// handler is not present in the application code.
//
//*****************************************************************************
__attribute__ ((section(".after_vectors")))
void IntDefaultHandler(void)
{
    while(1)
    {
    }
}


