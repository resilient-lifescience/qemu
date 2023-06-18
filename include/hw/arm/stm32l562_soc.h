#ifndef HW_ARM_STM32L562_SOC_H
#define HW_ARM_STM32L562_SOC_H

#include "hw/char/stm32l5xx_usart.h"
#include "hw/misc/stm32l5xx_pwr.h"
#include "hw/or-irq.h"
#include "hw/arm/armv7m.h"
#include "hw/clock.h"
#include "qom/object.h"

#define TYPE_STM32L562_SOC "stm32l562-soc"
OBJECT_DECLARE_SIMPLE_TYPE(STM32L562State, STM32L562_SOC)

// Memory regions: Should match linker script (and the device).
#define FLASH_BASE_ADDRESS 0x08000000
#define FLASH_SIZE (512 * 1024)
#define RAM_BASE_ADDRESS 0x20000000
#define RAM_SIZE (256 * 1024)

struct STM32L562State
{
    /*< private >*/
    SysBusDevice parent_obj;
    /*< public >*/

    char *cpu_type;

    ARMv7MState armv7m;

    STM32L5XXUsartState usart;
    STM32L5XXPwrState pwr;

    MemoryRegion ram;
    MemoryRegion flash;
    MemoryRegion flash_alias;

    Clock *sysclk;
};

#endif
