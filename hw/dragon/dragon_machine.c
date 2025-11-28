#include"qemu/osdep.h"
#include "hw/boards.h"
#include "qom/object.h"

struct DragonMachineState {
    MachineState parent_obj;
};

struct DragonMachineClass {
    MachineClass parent_class;
};

#define TYPE_DRAGON_MACHINE MACHINE_TYPE_NAME("dragon")

OBJECT_DECLARE_TYPE(DragonMachineState, DragonMachineClass, DRAGON_MACHINE)

static void dragaon_machine_class_init(ObjectClass *klass, const void *data) {
    MachineClass *mc = MACHINE_CLASS(klass);
    mc->desc = "Dragon Machine";
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