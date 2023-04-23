#ifndef HW_STM32L4XX_SYSCFG_H
#define HW_STM32L4XX_SYSCFG_H

#include "hw/sysbus.h"
#include "qom/object.h"

#define SYSCFG_MEMRMP 0x00
#define SYSCFG_CFGR1 0x04
#define SYSCFG_EXTICR1 0x08
#define SYSCFG_EXTICR2 0x0C
#define SYSCFG_EXTICR3 0x10
#define SYSCFG_EXTICR4 0x14
#define SYSCFG_SCSR 0x18
#define SYSCFG_CFGR2 0x1C
#define SYSCFG_SWPR 0x20
#define SYSCFG_SKR 0x24
#define SYSCFG_SWPR2 0x29

#define TYPE_STM32L4XX_SYSCFG "stm32l4xx-syscfg"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L4XXSyscfgState, STM32L4XX_SYSCFG)

struct STM32L4XXSyscfgState
{
    /* <private> */
    SysBusDevice parent_obj;

    /* <public> */
    MemoryRegion mmio;

    uint32_t syscfg_memrmp;
    uint32_t syscfg_cfgr1;
    uint32_t syscfg_exticr1;
    uint32_t syscfg_exticr2;
    uint32_t syscfg_exticr3;
    uint32_t syscfg_exticr4;
    uint32_t syscfg_scsr;
    uint32_t syscfg_cfgr2;
    uint32_t syscfg_swpr;
    uint32_t syscfg_skr;
    uint32_t syscfg_swpr2;
};

#endif /* HW_STM32L4XX_SYSCFG_H */
