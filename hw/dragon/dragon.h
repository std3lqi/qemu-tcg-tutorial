#ifndef HW_DRAGON_CPU_H
#define HW_DRAGON_CPU_H

#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#include "target/dragon/cpu.h"
#include "target/dragon/cpu-qom.h"
#include "system/memory.h"

struct DragonCPUState {
    SysBusDevice parent_obj;
    DragonArchCPU cpu;
    MemoryRegion flash;
};

struct DragonCPUClass {
    SysBusDeviceClass parent_class;
    size_t flash_size;
};

#define TYPE_DRAGON_CPU "DRAGON_CPU"

OBJECT_DECLARE_TYPE(DragonCPUState, DragonCPUClass, DRAGON_CPU)

#endif // HW_DRAGON_CPU_H