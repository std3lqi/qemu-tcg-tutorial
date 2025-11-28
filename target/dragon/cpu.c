#include "qemu/osdep.h"
#include "cpu.h"
#include "qom/object.h"
#include "cpu-qom.h"

static const TypeInfo dragon_arch_cpu_types[] = {
    {
        .name = TYPE_DRAGON_ARCH_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(DragonArchCPU),
        .class_size = sizeof(DragonArchCPUClass),
    }
};

DEFINE_TYPES(dragon_arch_cpu_types)
