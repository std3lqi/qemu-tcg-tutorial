#include "qemu/osdep.h"
#include "disas/disas.h"
#include "cpu.h"

#define INSN_LEN 4

typedef struct DisasContext {
    disassemble_info *dis;
    uint64_t addr;
    uint8_t bytes[INSN_LEN];
} DisasContext;

static inline int shl_2(DisasContext *ctx, int x) {
    return x << 2;
}

static uint32_t read_bytes(DisasContext *ctx) {
    ctx->dis->read_memory_func(ctx->addr, ctx->bytes, INSN_LEN, ctx->dis);
    uint32_t insn = bfd_getl32(ctx->bytes);
    ctx->addr += INSN_LEN;
    return insn;
}

static bool decode_insn(DisasContext *ctx, uint32_t insn);
#include "decode-insn.c.inc"

int dragon_arch_cpu_print_insn(bfd_vma addr, struct disassemble_info *info) {
    DisasContext ctx = {
        .dis = info,
        .addr = addr,
    };
    uint32_t insn = read_bytes(&ctx);
    bool decoded = decode_insn(&ctx, insn);
    if (!decoded) {
        ctx.dis->fprintf_func(ctx.dis->stream, ".byte\t");
        for (int i = 0; i < INSN_LEN; i++) {
            // AABBCCDD
            ctx.dis->fprintf_func(ctx.dis->stream, "%02x", insn >> 24); // AA
            insn <<= 8;    // BBCCDD00 => CCDD0000 => DD000000
        }
    }
    return INSN_LEN;
}

#define REG(x) dragon_arch_cpu_gpr_names[x]

#define INSN(opcode, mnemonic, format, ...)                             \
static bool trans_##opcode(DisasContext *ctx, arg_##opcode *a) {        \
    ctx->dis->fprintf_func(ctx->dis->stream, "%-12s" format, mnemonic,   \
        ##__VA_ARGS__);                                                 \
    return true;                                                        \
}

INSN(ADD_W,     "add.w",    "$%s, $%s, $%s",    REG(a->rd), REG(a->rj), REG(a->rk))
INSN(ADD_D,     "add.d",    "$%s, $%s, $%s",    REG(a->rd), REG(a->rj), REG(a->rk))
INSN(ADDI_W,    "addi.w",   "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(ADDI_D,    "addi.d",   "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(LU12I_W,   "lu12i.w",  "$%s, 0x%X",        REG(a->rd), a->si20)
INSN(LU32I_D,   "lu32i.d",  "$%s, 0x%X",        REG(a->rd), a->si20)
INSN(BITREV_W,  "bitrev.w", "$%s, $%s",         REG(a->rd), REG(a->rj))
INSN(BITREV_D,  "bitrev.d", "$%s, $%s",         REG(a->rd), REG(a->rj))
INSN(LD_W,      "ld.w",     "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(LD_D,      "ld.d",     "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(ST_W,      "st.w",     "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(ST_D,      "st.d",     "$%s, $%s, 0x%X",   REG(a->rd), REG(a->rj), a->si12)
INSN(BEQ,       "beq",      "$%s, $%s, %d",     REG(a->rj), REG(a->rd), a->offs)
INSN(B,         "b",        "%d",               a->offs)
