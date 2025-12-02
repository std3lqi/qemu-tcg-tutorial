#include "qemu/osdep.h"
#include "qom/object.h"
#include "hw/sysbus.h"
#include "dragon.h"
#include "qemu/units.h"
#include "qapi/error.h"
#include "system/memory.h"
#include "system/address-spaces.h"

static void dragon_cpu_realize(DeviceState *dev, Error **errp) {
    DragonCPUState *cpu_state = DRAGON_CPU(dev);
    DragonCPUClass *cpu_class = DRAGON_CPU_GET_CLASS(dev);

    // Realize CPU
    object_initialize_child(OBJECT(dev), "cpu", &cpu_state->cpu, 
                            TYPE_DRAGON_ARCH_CPU);
    // bool object_property_set_bool(Object *obj, const char *name,
    //                               bool value, Error **errp);
    object_property_set_bool(OBJECT(&cpu_state->cpu), "realized", true, 
                             &error_abort);

    // Realize Flash memory
    // bool memory_region_init_rom(MemoryRegion *mr,
    //                         Object *owner,
    //                         const char *name,
    //                         uint64_t size,
    //                         Error **errp);
    memory_region_init_rom(&cpu_state->flash, OBJECT(dev), "flash", 
                           cpu_class->flash_size, &error_abort);
    // void memory_region_add_subregion(MemoryRegion *mr,
    //                                 hwaddr offset,
    //                                 MemoryRegion *subregion);
    memory_region_add_subregion(get_system_memory(), 0xC0000000,
                                &cpu_state->flash);
    memory_region_init_ram(&cpu_state->memory, OBJECT(dev), "memory", 
                           cpu_class->memory_size, &error_abort);
    memory_region_add_subregion(get_system_memory(), 0xffffffffd0000000,
                                &cpu_state->memory);
}

static void dragon_cpu_class_init(ObjectClass *klass, const void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);
    dc->realize = dragon_cpu_realize;

    DragonCPUClass *cpu_class = DRAGON_CPU_CLASS(klass);
    cpu_class->flash_size = 1 * MiB;
    cpu_class->memory_size = 1 * MiB;
}

static const TypeInfo dragon_cpu_types[] = {
    {
        .name = TYPE_DRAGON_CPU,
        .parent = TYPE_SYS_BUS_DEVICE,
        .instance_size = sizeof(DragonCPUState),
        .class_size = sizeof(DragonCPUClass),
        .class_init = dragon_cpu_class_init,
    }
};

DEFINE_TYPES(dragon_cpu_types)
