#include "qemu/osdep.h"
#include "qapi/error.h"
#include "qemu/module.h"
#include "hw/arm/boot.h"
#include "exec/address-spaces.h"
#include "hw/arm/stm32l432_soc.h"
#include "hw/qdev-properties.h"
#include "hw/qdev-clock.h"
#include "sysemu/sysemu.h"

static const uint32_t usart_addr[STM_NUM_USARTS] = {0x40013800, 0x40004400};

static const int usart_irq[STM_NUM_USARTS] = {37, 38};

static void stm32l432_soc_initfn(Object *obj)
{
    STM32L432State *s = STM32L432_SOC(obj);
    int i;

    object_initialize_child(obj, "armv7m", &s->armv7m, TYPE_ARMV7M);

    object_initialize_child(obj, "syscfg", &s->syscfg, TYPE_STM32L4XX_SYSCFG);

    for (i = 0; i < STM_NUM_USARTS; i++)
    {
        object_initialize_child(obj, "usart[*]", &s->usart[i],
                                TYPE_STM32L4XX_USART);
    }

    s->sysclk = qdev_init_clock_in(DEVICE(s), "sysclk", NULL, NULL, 0);
    s->refclk = qdev_init_clock_in(DEVICE(s), "refclk", NULL, NULL, 0);
}

static void stm32l432_soc_realize(DeviceState *dev_soc, Error **errp)
{
    STM32L432State *s = STM32L432_SOC(dev_soc);
    DeviceState *dev, *armv7m;
    SysBusDevice *busdev;
    int i;

    MemoryRegion *system_memory = get_system_memory();

    /*
     * We use s->refclk internally and only define it with qdev_init_clock_in()
     * so it is correctly parented and not leaked on an init/deinit; it is not
     * intended as an externally exposed clock.
     */
    if (clock_has_source(s->refclk))
    {
        error_setg(errp, "refclk clock must not be wired up by the board code");
        return;
    }

    if (!clock_has_source(s->sysclk))
    {
        error_setg(errp, "sysclk clock must be wired up by the board code");
        return;
    }

    /*
     * TODO: ideally we should model the SoC RCC and its ability to
     * change the sysclk frequency and define different sysclk sources.
     */

    /* The refclk always runs at frequency HCLK / 8 */
    clock_set_mul_div(s->refclk, 8, 1);
    clock_set_source(s->refclk, s->sysclk);

    memory_region_init_rom(&s->flash, OBJECT(dev_soc), "STM32L432.flash",
                           FLASH_SIZE, &error_fatal);
    memory_region_init_alias(&s->flash_alias, OBJECT(dev_soc),
                             "STM32L432.flash.alias", &s->flash, 0, FLASH_SIZE);

    memory_region_add_subregion(system_memory, FLASH_BASE_ADDRESS, &s->flash);
    memory_region_add_subregion(system_memory, 0, &s->flash_alias);

    memory_region_init_ram(&s->ram, NULL, "STM32L432.ram", RAM_SIZE,
                           &error_fatal);
    memory_region_add_subregion(system_memory, RAM_BASE_ADDRESS, &s->ram);

    armv7m = DEVICE(&s->armv7m);
    qdev_prop_set_uint32(armv7m, "num-irq", 96);
    qdev_prop_set_string(armv7m, "cpu-type", s->cpu_type);
    qdev_prop_set_bit(armv7m, "enable-bitband", true);
    qdev_connect_clock_in(armv7m, "cpuclk", s->sysclk);
    qdev_connect_clock_in(armv7m, "refclk", s->refclk);
    object_property_set_link(OBJECT(&s->armv7m), "memory",
                             OBJECT(get_system_memory()), &error_abort);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->armv7m), errp))
    {
        return;
    }

    /* System configuration controller */
    dev = DEVICE(&s->syscfg);
    if (!sysbus_realize(SYS_BUS_DEVICE(&s->syscfg), errp))
    {
        return;
    }
    busdev = SYS_BUS_DEVICE(dev);
    sysbus_mmio_map(busdev, 0, 0x40010000);

    /* Attach UART (uses USART registers) and USART controllers */
    for (i = 0; i < STM_NUM_USARTS; i++)
    {
        dev = DEVICE(&(s->usart[i]));
        qdev_prop_set_chr(dev, "chardev", serial_hd(i));
        if (!sysbus_realize(SYS_BUS_DEVICE(&s->usart[i]), errp))
        {
            return;
        }
        busdev = SYS_BUS_DEVICE(dev);
        sysbus_mmio_map(busdev, 0, usart_addr[i]);
        sysbus_connect_irq(busdev, 0, qdev_get_gpio_in(armv7m, usart_irq[i]));
    }
}

static Property stm32l432_soc_properties[] = {
    DEFINE_PROP_STRING("cpu-type", STM32L432State, cpu_type),
    DEFINE_PROP_END_OF_LIST(),
};

static void stm32l432_soc_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->realize = stm32l432_soc_realize;
    device_class_set_props(dc, stm32l432_soc_properties);
}

static const TypeInfo stm32l432_soc_info = {
    .name = TYPE_STM32L432_SOC,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L432State),
    .instance_init = stm32l432_soc_initfn,
    .class_init = stm32l432_soc_class_init,
};

static void stm32l432_soc_types(void)
{
    type_register_static(&stm32l432_soc_info);
}

type_init(stm32l432_soc_types)
