#ifndef HW_STM32L4XX_USART_H
#define HW_STM32L4XX_USART_H

#include "hw/sysbus.h"
#include "chardev/char-fe.h"
#include "qom/object.h"

#define USART_CR1 0x00
#define USART_CR2 0x04
#define USART_CR3 0x08
#define USART_BRR 0x0C
#define USART_GTPR 0x10
#define USART_RTOR 0x14
#define USART_RQR 0x18
#define USART_ISR 0x1C
#define USART_ICR 0x20
#define USART_RDR 0x24
#define USART_TDR 0x28
#define USART_PRESC 0x2C

#define USART_ISR_RESET (USART_ISR_TXE | USART_ISR_TC)

#define USART_ISR_TXE (1 << 7)
#define USART_ISR_TC (1 << 6)
#define USART_ISR_RXNE (1 << 5)

#define USART_CR1_RXNEIE (1 << 5)
#define USART_CR1_TE (1 << 3)
#define USART_CR1_RE (1 << 2)
#define USART_CR1_UE 1

#define TYPE_STM32L4XX_USART "stm32l4xx-usart"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L4XXUsartState, STM32L4XX_USART)

struct STM32L4XXUsartState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t usart_cr1;
    uint32_t usart_cr2;
    uint32_t usart_cr3;
    uint32_t usart_brr;
    uint32_t usart_gtpr;
    uint32_t usart_rtor;
    uint32_t usart_rqr;
    uint32_t usart_isr;
    uint32_t usart_icr;
    uint32_t usart_rdr;
    uint32_t usart_tdr;
    uint32_t usart_presc;

    CharBackend chr;
    qemu_irq irq;
};
#endif /* HW_STM32L4XX_USART_H */
