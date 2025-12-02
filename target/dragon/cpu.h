#ifndef DRAGON_CPU_H
#define DRAGON_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"
#include "hw/core/cpu.h"

#define CPU_RESOLVING_TYPE TYPE_DRAGON_ARCH_CPU

#define NUM_OF_GPR 32

typedef struct CPUArchState {
    uint64_t gpr[NUM_OF_GPR];
    uint64_t pc;                // Program Counter
} DragonCPUArchState;

struct ArchCPU {
    CPUState parent_obj;
    DragonCPUArchState env;
};

static const char dragon_arch_cpu_gpr_names[NUM_OF_GPR][8] = {
    "r0","r1","r2","r3","r4","r5","r6","r7",
    "r8","r9","r10","r11","r12","r13","r14","r15",
    "r16","r17","r18","r19","r20","r21","r22","r23",
    "r24","r25","r26","r27","r28","r29","r30","r31"
};

void dragon_tcg_init(void);
void dragon_tcg_translate_code(CPUState *cpu, TranslationBlock *tb,
                           int *max_insns, vaddr pc, void *host_pc);
int dragon_arch_cpu_print_insn(bfd_vma addr, struct disassemble_info *info);
#endif // DRAGON_CPU_H