#include"qemu/osdep.h"
#include "hw/boards.h"
#include "qom/object.h"
#include "dragon.h"
#include "qapi/error.h"
#include "boot.h"

struct DragonMachineState {
    MachineState parent_obj;
    DragonCPUState cpu;
};

struct DragonMachineClass {
    MachineClass parent_class;
};

#define TYPE_DRAGON_MACHINE MACHINE_TYPE_NAME("dragon")

OBJECT_DECLARE_TYPE(DragonMachineState, DragonMachineClass, DRAGON_MACHINE)

static void dragon_machine_init(MachineState *state) {
    DragonMachineState *machine_state = DRAGON_MACHINE(state);
    // Initialize CPU QOM object
    object_initialize_child(OBJECT(state), "cpu", &machine_state->cpu, 
                            TYPE_DRAGON_CPU);

    // Realize CPU on sysbus
    // bool sysbus_realize(SysBusDevice *dev, Error **errp);
    sysbus_realize(SYS_BUS_DEVICE(&machine_state->cpu), &error_abort);

    if (state->firmware) {
        bool loaded = dragon_load_firmware(&machine_state->cpu.flash, 
            state->firmware);
        if (!loaded) {
            exit(1);
        }
    }
}

static void dragaon_machine_class_init(ObjectClass *klass, const void *data) {
    MachineClass *mc = MACHINE_CLASS(klass);
    mc->desc = "Dragon Machine";
    mc->init = dragon_machine_init;
    mc->default_cpus = 1;
    mc->min_cpus = 1;
    mc->max_cpus = 1;
}

static const TypeInfo dragon_machine_types[] = {
    {
        .name = TYPE_DRAGON_MACHINE,
        .parent = TYPE_MACHINE,
        .instance_size = sizeof(DragonMachineState),
        .class_size = sizeof(DragonMachineClass),
        .class_init = dragaon_machine_class_init,
    }
};

DEFINE_TYPES(dragon_machine_types)