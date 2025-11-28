#ifndef DRAGON_CPU_H
#define DRAGON_CPU_H

#include "cpu-qom.h"
#include "exec/cpu-defs.h"
#include "hw/core/cpu.h"

#define CPU_RESOLVING_TYPE TYPE_DRAGON_ARCH_CPU

typedef struct CPUArchState {

} DragonCPUArchState;

struct ArchCPU {
    CPUState parent_obj;
    DragonCPUArchState env;
};

#endif // DRAGON_CPU_H