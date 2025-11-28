#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#include "dragon.h"

static const TypeInfo dragon_cpu_types[] = {
    {
        .name = TYPE_DRAGON_CPU,
        .parent = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(DragonCPUState),
        .class_size = sizeof(DragonCPUClass),
    }
};

DEFINE_TYPES(dragon_cpu_types)
