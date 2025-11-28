#ifndef HW_DRAGON_CPU_H
#define HW_DRAGON_CPU_H

#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/sysbus.h"

struct DragonCPUState {
    SysBusDevice parent_obj;
};

struct DragonCPUClass {
    SysBusDeviceClass parent_class;
};

#define TYPE_DRAGON_CPU "DRAGON_CPU"

OBJECT_DECLARE_TYPE(DragonCPUState, DragonCPUClass, DRAGON_CPU)

#endif // HW_DRAGON_CPU_H