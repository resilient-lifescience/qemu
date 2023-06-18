#include "qemu/osdep.h"
#include "hw/char/stm32l5xx_usart.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"

static void stm32l5xx_usart_reset(DeviceState *dev)
{
    STM32L5XXUsartState *s = STM32L5XX_USART(dev);
    s->usart_tdr = 0;
}

static void stm32l5xx_usart_write(void *opaque, hwaddr addr,
                                  uint64_t val64, unsigned int size)
{
    STM32L5XXUsartState *s = opaque;

    // Whenever a character is written to USART's TDR register, print it to
    // console.
    if (addr == USART_TDR)
    {
        unsigned char ch = val64;
        qemu_chr_fe_write_all(&s->chr, &ch, 1);
    }
}

static const MemoryRegionOps stm32l5xx_usart_ops = {
    .write = stm32l5xx_usart_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static Property stm32l5xx_usart_properties[] = {
    DEFINE_PROP_CHR("chardev", STM32L5XXUsartState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32l5xx_usart_init(Object *obj)
{
    STM32L5XXUsartState *s = STM32L5XX_USART(obj);

    memory_region_init_io(&s->mmio, obj, &stm32l5xx_usart_ops, s,
                          TYPE_STM32L5XX_USART, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32l5xx_usart_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32l5xx_usart_reset;
    device_class_set_props(dc, stm32l5xx_usart_properties);
}

static const TypeInfo stm32l5xx_usart_info = {
    .name = TYPE_STM32L5XX_USART,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L5XXUsartState),
    .instance_init = stm32l5xx_usart_init,
    .class_init = stm32l5xx_usart_class_init,
};

static void stm32l5xx_usart_register_types(void)
{
    type_register_static(&stm32l5xx_usart_info);
}

type_init(stm32l5xx_usart_register_types)
