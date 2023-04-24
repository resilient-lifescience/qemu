/*
This is meant to be the simplest possible way for the guest system (our code)
to output characters to the console (e.g. to show test results).

We borrow a single USART register register for this purpose: USART_TDR.
When the guest system writes a character to that register, qemu will print
it to the console. Example usage in the guest system:

    #define USART_ADDR 0x40013800
    #define USART_TDR (USART_ADDR + 0x28)
    *((char*)USART_TDR) = 'a';
    *((char*)USART_TDR) = 'b';
*/
#ifndef HW_STM32L4XX_USART_H
#define HW_STM32L4XX_USART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

#define USART_TDR 0x28

#define TYPE_STM32L4XX_USART "stm32l4xx-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L4XXUsartState, STM32L4XX_USART)

struct STM32L4XXUsartState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    // USART_TDR: Transmit data register (0x28)
    // Contains the data character to be transmitted.
    uint32_t usart_tdr;

    CharBackend chr;
};
#endif /* HW_STM32L4XX_USART_H */
