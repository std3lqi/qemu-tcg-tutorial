#include "qemu/osdep.h"
#include "exec/helper-proto.h"
#include "cpu.h"

// DEF_HELPER_3(bitrev_w, void, env, i32, i32)
void helper_bitrev_w(DragonCPUArchState *env, uint32_t rd, uint32_t rj) {
    // Input: 0010
    // Outpu: 0100
    // bstr32[31:0] = BITREV(GR[rj][31:0])
    uint64_t input = env->gpr[rj] & 0x00000000ffffffff;
    uint64_t output = 0;
    uint32_t input_mask = 0x00000001;
    uint32_t output_mask = 0x80000000;
    for (int i = 0; i < 32; i++) {
        if ((input & input_mask) != 0) {
            output |= output_mask;
        }
        input_mask = input_mask << 1;
        output_mask = output_mask >> 1;
    }
    env->gpr[rd] = output;
    // GR[rd] = SignExtend(bstr32, GRLEN)
    if (env->gpr[rd] & 0x80000000) {
        env->gpr[rd] |= 0xffffffff00000000;
    }
}

// DEF_HELPER_3(bitrev_d, void, env, i32, i32)
void helper_bitrev_d(DragonCPUArchState *env, uint32_t rd, uint32_t rj) {
    // Input: 0010
    // Outpu: 0100
    // GR[rd] = BITREV(GR[rj][63:0])
    uint64_t input = env->gpr[rj] & 0x00000000ffffffff;
    uint64_t output = 0;
    uint64_t input_mask = 0x0000000000000001;
    uint64_t output_mask = 0x8000000000000000;
    for (int i = 0; i < 64; i++) {
        if ((input & input_mask) != 0) {
            output |= output_mask;
        }
        input_mask = input_mask << 1;
        output_mask = output_mask >> 1;
    }
    env->gpr[rd] = output;
}