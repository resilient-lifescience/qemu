#ifndef HW_ARM_STM32L432_SOC_H
#define HW_ARM_STM32L432_SOC_H

#include "hw/misc/stm32l4xx_syscfg.h"
#include "hw/char/stm32l4xx_usart.h"
#include "hw/or-irq.h"
#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"

#define TYPE_STM32L432_SOC "stm32l432-soc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L432State, STM32L432_SOC)

#define STM_NUM_USARTS 2

#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (256 * 1024)
#define RAM_BASE_ADDRESS 0x20000000
#define RAM_SIZE (64 * 1024)

struct STM32L432State
{
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/

    char *cpu_type;

    ARMv7MState armv7m;

    STM32L4XXSyscfgState syscfg;
    STM32L4XXUsartState usart[STM_NUM_USARTS];

    MemoryRegion ram;
    MemoryRegion flash;
    MemoryRegion flash_alias;

    Clock *sysclk;
    Clock *refclk;
};

#endif
