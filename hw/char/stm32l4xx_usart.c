#include "qemu/osdep.h"
#include "hw/char/stm32l4xx_usart.h"
#include "hw/irq.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef STM_USART_ERR_DEBUG
#define STM_USART_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do                                              \
    {                                               \
        if (STM_USART_ERR_DEBUG >= lvl)             \
        {                                           \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)

static int stm32l4xx_usart_can_receive(void *opaque)
{
    STM32L4XXUsartState *s = opaque;
    return !(s->usart_isr & USART_ISR_RXNE);
}

static void stm32l4xx_usart_receive(void *opaque, const uint8_t *buf, int size)
{
    STM32L4XXUsartState *s = opaque;

    if (!(s->usart_cr1 & USART_CR1_UE && s->usart_cr1 & USART_CR1_RE))
    {
        /* USART not enabled - drop the chars */
        DB_PRINT("Dropping the chars\n");
        return;
    }

    s->usart_rdr = *buf;
    s->usart_isr |= USART_ISR_RXNE;

    DB_PRINT("Receiving: %c\n", s->usart_rdr);
}

static void stm32l4xx_usart_reset(DeviceState *dev)
{
    STM32L4XXUsartState *s = STM32L4XX_USART(dev);

    s->usart_isr = USART_ISR_RESET;
    s->usart_cr1 = 0x00000000;
    s->usart_cr2 = 0x00000000;
    s->usart_cr3 = 0x00000000;
    s->usart_brr = 0x00000000;
    s->usart_gtpr = 0x00000000;
    s->usart_rtor = 0x00000000;
    s->usart_rqr = 0x00000000;
    s->usart_icr = 0x00000000;
    s->usart_rdr = 0x00000000;
    s->usart_tdr = 0x00000000;
    s->usart_presc = 0x00000000;
}

static uint64_t stm32l4xx_usart_read(void *opaque, hwaddr addr,
                                     unsigned int size)
{
    STM32L4XXUsartState *s = opaque;
    uint64_t retvalue;

    DB_PRINT("Read 0x%" HWADDR_PRIx "\n", addr);

    switch (addr)
    {
    case USART_RDR:
        DB_PRINT("Value: 0x%" PRIx32 ", %c\n", s->usart_rdr, (char)s->usart_rdr);
        retvalue = s->usart_rdr & 0x1FF;
        s->usart_isr &= ~USART_ISR_RXNE;
        qemu_chr_fe_accept_input(&s->chr);
        return retvalue;
    case USART_TDR:
        return s->usart_tdr;
    case USART_ISR:
        return s->usart_isr;
    case USART_ICR:
        return s->usart_icr;
    case USART_CR1:
        return s->usart_cr1;
    case USART_CR2:
        return s->usart_cr2;
    case USART_CR3:
        return s->usart_cr3;
    case USART_BRR:
        return s->usart_brr;
    case USART_GTPR:
        return s->usart_gtpr;
    case USART_RTOR:
        return s->usart_rtor;
    case USART_RQR:
        return s->usart_rqr;
    case USART_PRESC:
        return s->usart_presc;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
        return 0;
    }

    return 0;
}

static void stm32l4xx_usart_write(void *opaque, hwaddr addr,
                                  uint64_t val64, unsigned int size)
{
    STM32L4XXUsartState *s = opaque;
    uint32_t value = val64;
    unsigned char ch;

    DB_PRINT("Write 0x%" PRIx32 ", 0x%" HWADDR_PRIx "\n", value, addr);

    switch (addr)
    {
    case USART_RDR:
        s->usart_rdr = value;
        return;
    case USART_TDR:
        ch = value;
        qemu_chr_fe_write_all(&s->chr, &ch, 1);
        s->usart_isr |= USART_ISR_TC;
        s->usart_isr ^= ~USART_ISR_TXE;
        return;
    case USART_ISR:
        return;
    case USART_ICR:
        // Clear the corresponding status register bits.
        s->usart_isr &= ~value;
        return;
    case USART_CR1:
        s->usart_cr1 = value;
        return;
    case USART_CR2:
        s->usart_cr2 = value;
        return;
    case USART_CR3:
        s->usart_cr3 = value;
        return;
    case USART_BRR:
        s->usart_brr = value;
        return;
    case USART_GTPR:
        s->usart_gtpr = value;
        return;
    case USART_RTOR:
        s->usart_rtor = value;
        return;
    case USART_RQR:
        s->usart_rqr = value;
        return;
    case USART_PRESC:
        s->usart_presc = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps stm32l4xx_usart_ops = {
    .read = stm32l4xx_usart_read,
    .write = stm32l4xx_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32l4xx_usart_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32L4XXUsartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32l4xx_usart_init(Object *obj)
{
    STM32L4XXUsartState *s = STM32L4XX_USART(obj);

    sysbus_init_irq(SYS_BUS_DEVICE(obj), &s->irq);

    memory_region_init_io(&s->mmio, obj, &stm32l4xx_usart_ops, s,
                          TYPE_STM32L4XX_USART, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32l4xx_usart_realize(DeviceState *dev, Error **errp)
{
    STM32L4XXUsartState *s = STM32L4XX_USART(dev);

    qemu_chr_fe_set_handlers(&s->chr, stm32l4xx_usart_can_receive,
                             stm32l4xx_usart_receive, NULL, NULL,
                             s, NULL, true);
}

static void stm32l4xx_usart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32l4xx_usart_reset;
    device_class_set_props(dc, stm32l4xx_usart_properties);
    dc->realize = stm32l4xx_usart_realize;
}

static const TypeInfo stm32l4xx_usart_info = {
    .name = TYPE_STM32L4XX_USART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L4XXUsartState),
    .instance_init = stm32l4xx_usart_init,
    .class_init = stm32l4xx_usart_class_init,
};

static void stm32l4xx_usart_register_types(void)
{
    type_register_static(&stm32l4xx_usart_info);
}

type_init(stm32l4xx_usart_register_types)
