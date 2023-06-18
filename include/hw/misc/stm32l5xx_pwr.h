/*
This is meant to be the simplest possible way for the guest system (our code)
to trigger a qemu shutdown. The primary use case is during test: we want qemu
to exit when the test code is complete.

We borrow a single PWR register register for this purpose: PWR_CR1.
When the guest system writes 0bx1xx to that register, we trigger a qemu shutdown.

Example usage in the guest system:

    #define PWR_ADDR 0x50007000
    #define PWR_CR1 (USART_ADDR + 0x00)
    *((char*)USART_TDR) = 0b0100;
*/
#ifndef HW_STM32L5XX_PWR_H
#define HW_STM32L5XX_PWR_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define PWR_CR1 0x00

#define TYPE_STM32L5XX_PWR "stm32l5xx-pwr"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L5XXPwrState, STM32L5XX_PWR)

struct STM32L5XXPwrState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;
};
#endif /* HW_STM32L5XX_PWR_H */
