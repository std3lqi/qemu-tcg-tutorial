#include "qemu/osdep.h"
#include "cpu.h"
#include "qom/object.h"
#include "cpu-qom.h"
#include "qemu/qemu-print.h"
#include "qapi/error.h"

static void dragon_arch_cpu_init(Object *obj) {
    // Do nothing
}

static void dragon_arch_cpu_realize(DeviceState *dev, Error **errp) {
    CPUState *cpu = CPU(dev);
    DragonArchCPUClass *dragon = DRAGON_ARCH_CPU_GET_CLASS(dev);
    
    Error *local_err = NULL;
    cpu_exec_realizefn(cpu, &local_err);
    if (local_err != NULL) {
        error_propagate(errp, local_err);
        return;
    }

    qemu_init_vcpu(cpu);

    dragon->parent_realize(dev, errp);
}

static void dragon_arch_cpu_reset_hold(Object *obj, ResetType type) {
    DragonArchCPU *cpu = DRAGON_ARCH_CPU(obj);
    DragonCPUArchState *env = &cpu->env;
    // Initialize registers
    for (int i = 0; i < NUM_OF_GPR; i++) {
        env->gpr[i] = 0;
    }
    // Initialize program counter
    env->pc = 0xC0000000;

    DragonArchCPUClass *dragon = DRAGON_ARCH_CPU_GET_CLASS(obj);
    if (dragon->parent_phases.hold) {
        dragon->parent_phases.hold(obj, type);
    }
}

static ObjectClass *dragon_arch_cpu_class_by_name(const char *cpu_model) {
    return object_class_by_name(TYPE_DRAGON_ARCH_CPU);
}

static void dragon_arch_cpu_dump_state(CPUState *cpu, FILE *f, int flags) {
    DragonArchCPU *dragon = DRAGON_ARCH_CPU(cpu);
    DragonCPUArchState *env = &dragon->env;
    qemu_fprintf(f, "PC:    " TARGET_FMT_lx "\n", env->pc);
    for (int i = 0; i < NUM_OF_GPR; i++) {
        qemu_fprintf(f, "%s:    " TARGET_FMT_lx "\n", 
            dragon_arch_cpu_gpr_names[i], env->gpr[i]);
    }
    qemu_fprintf(f, "\n");
}

static void dragon_arch_cpu_set_pc(CPUState *cpu, vaddr value) {
    DragonArchCPU *dragon = DRAGON_ARCH_CPU(cpu);
    DragonCPUArchState *env = &dragon->env;
    env->pc = value;
}

static void dragon_arch_cpu_class_init(ObjectClass *klass, const void *data) {
    DeviceClass *dc = DEVICE_CLASS(klass);
    CPUClass *cc = CPU_CLASS(klass);
    DragonArchCPUClass *dragon = DRAGON_ARCH_CPU_CLASS(klass);
// void device_class_set_parent_realize(DeviceClass *dc,
//                                      DeviceRealize dev_realize,
//                                      DeviceRealize *parent_realize)
// {
//     *parent_realize = dc->realize;
//     dc->realize = dev_realize;
// }    
    device_class_set_parent_realize(dc, dragon_arch_cpu_realize, 
        &dragon->parent_realize);

// void resettable_class_set_parent_phases(ResettableClass *rc,
//                                         ResettableEnterPhase enter,
//                                         ResettableHoldPhase hold,
//                                         ResettableExitPhase exit,
//                                         ResettablePhases *parent_phases);        
    ResettableClass *rc = RESETTABLE_CLASS(klass);
    resettable_class_set_parent_phases(rc, NULL, dragon_arch_cpu_reset_hold, 
        NULL, &dragon->parent_phases);

    // Initialize CPUClass
    cc->class_by_name = dragon_arch_cpu_class_by_name;
    cc->dump_state = dragon_arch_cpu_dump_state;
    cc->set_pc = dragon_arch_cpu_set_pc;
}

static const TypeInfo dragon_arch_cpu_types[] = {
    {
        .name = TYPE_DRAGON_ARCH_CPU,
        .parent = TYPE_CPU,
        .instance_size = sizeof(DragonArchCPU),
        .instance_init = dragon_arch_cpu_init,
        .class_size = sizeof(DragonArchCPUClass),
        .class_init = dragon_arch_cpu_class_init,
    }
};

DEFINE_TYPES(dragon_arch_cpu_types)
