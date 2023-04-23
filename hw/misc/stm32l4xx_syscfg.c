#include "qemu/osdep.h"
#include "hw/misc/stm32l4xx_syscfg.h"
#include "qemu/log.h"
#include "qemu/module.h"

#ifndef STM_SYSCFG_ERR_DEBUG
#define STM_SYSCFG_ERR_DEBUG 0
#endif

#define DB_PRINT_L(lvl, fmt, args...)               \
    do                                              \
    {                                               \
        if (STM_SYSCFG_ERR_DEBUG >= lvl)            \
        {                                           \
            qemu_log("%s: " fmt, __func__, ##args); \
        }                                           \
    } while (0)

#define DB_PRINT(fmt, args...) DB_PRINT_L(1, fmt, ##args)

static void stm32l4xx_syscfg_reset(DeviceState *dev)
{
    STM32L4XXSyscfgState *s = STM32L4XX_SYSCFG(dev);

    s->syscfg_memrmp = 0x00000000;
    s->syscfg_cfgr1 = 0x00000000;
    s->syscfg_exticr1 = 0x00000000;
    s->syscfg_exticr2 = 0x00000000;
    s->syscfg_exticr3 = 0x00000000;
    s->syscfg_exticr4 = 0x00000000;
    s->syscfg_scsr = 0x00000000;
    s->syscfg_cfgr2 = 0x00000000;
    s->syscfg_swpr = 0x00000000;
    s->syscfg_skr = 0x00000000;
    s->syscfg_swpr2 = 0x00000000;
}

static uint64_t stm32l4xx_syscfg_read(void *opaque, hwaddr addr,
                                      unsigned int size)
{
    STM32L4XXSyscfgState *s = opaque;

    DB_PRINT("0x%" HWADDR_PRIx "\n", addr);

    switch (addr)
    {
    case SYSCFG_MEMRMP:
        return s->syscfg_memrmp;
    case SYSCFG_CFGR1:
        return s->syscfg_cfgr1;
    case SYSCFG_EXTICR1:
        return s->syscfg_exticr1;
    case SYSCFG_EXTICR2:
        return s->syscfg_exticr2;
    case SYSCFG_EXTICR3:
        return s->syscfg_exticr3;
    case SYSCFG_EXTICR4:
        return s->syscfg_exticr4;
    case SYSCFG_SCSR:
        return s->syscfg_scsr;
    case SYSCFG_CFGR2:
        return s->syscfg_cfgr2;
    case SYSCFG_SWPR:
        return s->syscfg_swpr;
    case SYSCFG_SKR:
        return s->syscfg_skr;
    case SYSCFG_SWPR2:
        return s->syscfg_swpr2;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
        return 0;
    }

    return 0;
}

static void stm32l4xx_syscfg_write(void *opaque, hwaddr addr,
                                   uint64_t val64, unsigned int size)
{
    STM32L4XXSyscfgState *s = opaque;
    uint32_t value = val64;

    DB_PRINT("0x%x, 0x%" HWADDR_PRIx "\n", value, addr);

    switch (addr)
    {
    case SYSCFG_MEMRMP:
        qemu_log_mask(LOG_UNIMP,
                      "%s: Changeing the memory mapping isn't supported "
                      "in QEMU\n",
                      __func__);
        return;
    case SYSCFG_CFGR1:
        s->syscfg_cfgr1 = value;
        return;
    case SYSCFG_EXTICR1:
        s->syscfg_exticr1 = (value & 0xFFFF);
        return;
    case SYSCFG_EXTICR2:
        s->syscfg_exticr2 = (value & 0xFFFF);
        return;
    case SYSCFG_EXTICR3:
        s->syscfg_exticr3 = (value & 0xFFFF);
        return;
    case SYSCFG_EXTICR4:
        s->syscfg_exticr4 = (value & 0xFFFF);
        return;
    case SYSCFG_SCSR:
        s->syscfg_scsr = value;
        return;
    case SYSCFG_CFGR2:
        s->syscfg_cfgr2 = value;
        return;
    case SYSCFG_SWPR:
        s->syscfg_swpr = value;
        return;
    case SYSCFG_SKR:
        s->syscfg_skr = value;
        return;
    case SYSCFG_SWPR2:
        s->syscfg_swpr2 = value;
        return;
    default:
        qemu_log_mask(LOG_GUEST_ERROR,
                      "%s: Bad offset 0x%" HWADDR_PRIx "\n", __func__, addr);
    }
}

static const MemoryRegionOps stm32l4xx_syscfg_ops = {
    .read = stm32l4xx_syscfg_read,
    .write = stm32l4xx_syscfg_write,
    .endianness = DEVICE_NATIVE_ENDIAN,
};

static void stm32l4xx_syscfg_init(Object *obj)
{
    STM32L4XXSyscfgState *s = STM32L4XX_SYSCFG(obj);

    memory_region_init_io(&s->mmio, obj, &stm32l4xx_syscfg_ops, s,
                          TYPE_STM32L4XX_SYSCFG, 0x400);
    sysbus_init_mmio(SYS_BUS_DEVICE(obj), &s->mmio);
}

static void stm32l4xx_syscfg_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);

    dc->reset = stm32l4xx_syscfg_reset;
}

static const TypeInfo stm32l4xx_syscfg_info = {
    .name = TYPE_STM32L4XX_SYSCFG,
    .parent = TYPE_SYS_BUS_DEVICE,
    .instance_size = sizeof(STM32L4XXSyscfgState),
    .instance_init = stm32l4xx_syscfg_init,
    .class_init = stm32l4xx_syscfg_class_init,
};

static void stm32l4xx_syscfg_register_types(void)
{
    type_register_static(&stm32l4xx_syscfg_info);
}

type_init(stm32l4xx_syscfg_register_types)
