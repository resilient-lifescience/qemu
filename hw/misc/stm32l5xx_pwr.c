#include "qemu/osdep.h"
#include "hw/misc/stm32l5xx_pwr.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-properties-system.h"
#include "qemu/log.h"
#include "qemu/module.h"
#include "sysemu/runstate.h"

static void stm32l5xx_pwr_write(void *opaque, hwaddr addr,
                                uint64_t val64, unsigned int size)
{
    // Writing 1xx to register CR1 causes the device to enter "shutdown" state,
    // which is the lowest power mode. We use that same signal to exit qemu.
    // This applies during test, when we want our test code to exit when complete.
    if (addr == PWR_CR1 && (val64 & 0b0100) == 0b0100)
    {
        qemu_system_shutdown_request(SHUTDOWN_CAUSE_GUEST_SHUTDOWN);
    }
}

static const MemoryRegionOps stm32l5xx_pwr_ops = {
    .write = stm32l5xx_pwr_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void stm32l5xx_pwr_init(Object *obj)
{
    STM32L5XXPwrState *s = STM32L5XX_PWR(obj);
    memory_region_init_io(&s->mmio, obj, &stm32l5xx_pwr_ops, s,
                          TYPE_STM32L5XX_PWR, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static const TypeInfo stm32l5xx_pwr_info = {
    .name = TYPE_STM32L5XX_PWR,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L5XXPwrState),
    .instance_init = stm32l5xx_pwr_init,
};

static void stm32l5xx_pwr_register_types(void)
{
    type_register_static(&stm32l5xx_pwr_info);
}

type_init(stm32l5xx_pwr_register_types)
