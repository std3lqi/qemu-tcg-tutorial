#include "qemu/osdep.h"
#include "cpu.h"
#include "qom/object.h"
#include "cpu-qom.h"
#include "qemu/qemu-print.h"
#include "qapi/error.h"
#include "hw/core/sysemu-cpu-ops.h"
#include "accel/tcg/cpu-ops.h"
#include "accel/tcg/tb-cpu-state.h"
#include "exec/cputlb.h"
#include "exec/target_page.h"

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
    cpu_reset(cpu);

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

static bool dragon_arch_cpu_has_work(CPUState *cpu) {
    return false;
}

static TCGTBCPUState dragon_tcg_get_tb_cpu_state(CPUState *cs) {
    DragonArchCPU *dragon = DRAGON_ARCH_CPU(cs);
    DragonCPUArchState *env = &dragon->env;
    return (TCGTBCPUState) {
        .pc = env->pc,
        .flags = 0,
        .cflags = 0,
        .cs_base = 0
    };
}

static int dragon_tcg_mmu_index(CPUState *cpu, bool ifetch) {
    return 0;
}

static void dragon_tcg_do_interrupt(CPUState *cpu) {
    // Do nothing
}

static bool dragon_tcg_cpu_exec_interrupt(CPUState *cpu, int interrupt_request) {
    return false;
}

static void dragon_tcg_cpu_exec_reset(CPUState *cpu) {
    // Do nothing
}

static bool dragon_tcg_cpu_exec_halt(CPUState *cpu) {
    return dragon_arch_cpu_has_work(cpu);
}

static bool dragon_tcg_tlb_fill(CPUState *cpu, vaddr address, int size,
                     MMUAccessType access_type, int mmu_idx,
                     bool probe, uintptr_t retaddr) {
    int prot = PAGE_READ | PAGE_WRITE | PAGE_EXEC;
// void tlb_set_page(CPUState *cpu, vaddr addr,
//                   hwaddr paddr, int prot,
//                   int mmu_idx, vaddr size)                        
    tlb_set_page(cpu, address, address, prot, mmu_idx, TARGET_PAGE_SIZE);
    return true;
}

static void dragon_tcg_restore_state_to_opc(CPUState *cpu, 
    const TranslationBlock *tb, const uint64_t *data) {
    DragonArchCPU *dragon = DRAGON_ARCH_CPU(cpu);
    dragon->env.pc = data[0];
}

static const struct SysemuCPUOps dragon_sysemu_ops = {
    .has_work = dragon_arch_cpu_has_work,
};

static const struct TCGCPUOps dragon_tcg_ops = {
    .initialize = dragon_tcg_init,
    .translate_code = dragon_tcg_translate_code,
    .get_tb_cpu_state = dragon_tcg_get_tb_cpu_state,
    .mmu_index = dragon_tcg_mmu_index,
    .do_interrupt = dragon_tcg_do_interrupt,
    .cpu_exec_interrupt = dragon_tcg_cpu_exec_interrupt,
    .cpu_exec_reset = dragon_tcg_cpu_exec_reset,
    .cpu_exec_halt = dragon_tcg_cpu_exec_halt,
    .tlb_fill = dragon_tcg_tlb_fill,
    .pointer_wrap = cpu_pointer_wrap_notreached,
    .restore_state_to_opc = dragon_tcg_restore_state_to_opc,
};

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

    cc->sysemu_ops = &dragon_sysemu_ops;
    cc->tcg_ops = &dragon_tcg_ops;
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
