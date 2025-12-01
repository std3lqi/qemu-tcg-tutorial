#include "qemu/osdep.h"
#include "disas/disas.h"
#include "exec/translator.h"
#include "cpu.h"

typedef struct DisasContext {
    DisasContextBase base;
} DisasContext;

static inline int shl_2(DisasContext *ctx, int x) {
    return x << 2;
}

bool decode_insn(DisasContext *ctx, uint32_t insn);
#include "decode-insn.c.inc"

static bool trans_ADD_W(DisasContext *ctx, arg_ADD_W *a) {
    return false;
}
static bool trans_ADD_D(DisasContext *ctx, arg_ADD_D *a) {
    return false;
}
static bool trans_ADDI_W(DisasContext *ctx, arg_ADDI_W *a) {
    return false;
}
static bool trans_ADDI_D(DisasContext *ctx, arg_ADDI_D *a) {
    return false;
}
static bool trans_LU12I_W(DisasContext *ctx, arg_LU12I_W *a) {
    return false;
}
static bool trans_LU32I_D(DisasContext *ctx, arg_LU32I_D *a) {
    return false;
}
static bool trans_BITREV_W(DisasContext *ctx, arg_BITREV_W *a) {
    return false;
}
static bool trans_BITREV_D(DisasContext *ctx, arg_BITREV_D *a) {
    return false;
}
static bool trans_LD_W(DisasContext *ctx, arg_LD_W *a) {
    return false;
}
static bool trans_LD_D(DisasContext *ctx, arg_LD_D *a) {
    return false;
}
static bool trans_ST_W(DisasContext *ctx, arg_ST_W *a) {
    return false;
}
static bool trans_ST_D(DisasContext *ctx, arg_ST_D *a) {
    return false;
}
static bool trans_BEQ(DisasContext *ctx, arg_BEQ *a) {
    return false;
}
static bool trans_B(DisasContext *ctx, arg_B *a) {
    return false;
}