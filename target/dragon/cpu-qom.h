#ifndef DRAGON_CPU_QOM_H
#define DRAGON_CPU_QOM_H

#include "qom/object.h"
#include "hw/core/cpu.h"

#define TYPE_DRAGON_ARCH_CPU "dragon-arch-cpu"

struct DragonArchCPUClass {
    CPUClass parent_class;
    DeviceRealize parent_realize;
    ResettablePhases parent_phases;
};

OBJECT_DECLARE_CPU_TYPE(DragonArchCPU, DragonArchCPUClass, DRAGON_ARCH_CPU)

#endif // DRAGON_CPU_QOM_H