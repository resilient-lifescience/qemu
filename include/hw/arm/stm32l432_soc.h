#ifndef HW_ARM_STM32L432_SOC_H
#define HW_ARM_STM32L432_SOC_H

#include "hw/char/stm32l4xx_usart.h"
#include "hw/misc/stm32l4xx_pwr.h"
#include "hw/or-irq.h"
#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"

#define TYPE_STM32L432_SOC "stm32l432-soc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L432State, STM32L432_SOC)

// Memory regions: Should match linker script (and the device).
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

    STM32L4XXUsartState usart;
    STM32L4XXPwrState pwr;

    MemoryRegion ram;
    MemoryRegion flash;
    MemoryRegion flash_alias;

    Clock *sysclk;
};

#endif
