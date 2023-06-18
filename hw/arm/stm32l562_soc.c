#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "hw/arm/stm32l562_soc.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "sysemu/sysemu.h"

#define USART_ADDR 0x50013800
#define PWR_ADDR 0x50007000

static void stm32l562_soc_initfn(Object *obj)
{
    STM32L562State *s = STM32L562_SOC(obj);

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);
    object_initialize_child(obj, "usart", &s->usart,
                            TYPE_STM32L5XX_USART);
    object_initialize_child(obj, "pwr", &s->pwr,
                            TYPE_STM32L5XX_PWR);

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
}

static void stm32l562_soc_realize(DeviceState *dev_soc, Error **errp)
{
    STM32L562State *s = STM32L562_SOC(dev_soc);
    DeviceState *dev, *armv7m;
    SysBusDevice *busdev;

    MemoryRegion *system_memory = get_system_memory();

    if (!clock_has_source(s->sysclk))
    {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "STM32L562.flash",
                           FLASH_SIZE, &error_fatal);
    memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, &s->flash);

    /* Alias flash start to address 0x00, so that qemu can find our first
    instruction. */
    memory_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "STM32L562.flash.alias", &s->flash, 0, FLASH_SIZE);
    memory_region_add_subregion(system_memory, 0, &s->flash_alias);

    memory_region_init_ram(&s->ram, NULL, "STM32L562.ram", RAM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, RAM_BASE_ADDRESS, &s->ram);

    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 96);
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(get_system_memory()), &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp))
    {
        return;
    }

    /* Attach UART (uses USART registers) and USART controllers */
    dev = DEVICE(&s->usart);
    qdev_prop_set_chr(dev, "chardev", serial_hd(0));
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->usart), errp))
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, USART_ADDR);

    /* Attach PWR controller */
    dev = DEVICE(&s->pwr);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->pwr), errp))
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, PWR_ADDR);
}

static Property stm32l562_soc_properties[] = {
    DEFINE_PROP_STRING("cpu-type", STM32L562State, cpu_type),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32l562_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = stm32l562_soc_realize;
    device_class_set_props(dc, stm32l562_soc_properties);
}

static const TypeInfo stm32l562_soc_info = {
    .name = TYPE_STM32L562_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L562State),
    .instance_init = stm32l562_soc_initfn,
    .class_init = stm32l562_soc_class_init,
};

static void stm32l562_soc_types(void)
{
    type_register_static(&stm32l562_soc_info);
}

type_init(stm32l562_soc_types)
