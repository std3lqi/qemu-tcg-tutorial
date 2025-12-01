#include "qemu/osdep.h"
#include "disas/disas.h"
#include "exec/translator.h"
#include "cpu.h"
#include "tcg/tcg-op.h"

static TCGv cpu_r[NUM_OF_GPR];
static TCGv pc;

typedef struct DisasContext {
    DisasContextBase base;
    DragonCPUArchState *env;
    uint64_t pc;
} DisasContext;

void dragon_tcg_init(void) {
    for (int i = 0; i < NUM_OF_GPR; i++) {
        // TCGv_i64 tcg_global_mem_new_i64(TCGv_ptr reg, intptr_t off, 
        //                                 const char *name)
        cpu_r[i] = tcg_global_mem_new(tcg_env,
                                      offsetof(DragonCPUArchState, gpr[i]),
                                      dragon_arch_cpu_gpr_names[i]);
    }
    pc = tcg_global_mem_new(tcg_env,
                            offsetof(DragonCPUArchState, pc),
                            "pc");
}

static void dragon_tcg_init_disas_context(DisasContextBase *db, CPUState *cpu) {
    DisasContext *ctx = container_of(db, DisasContext, base);
    ctx->env = cpu_env(cpu);
    ctx->pc = db->pc_first;
}

static void dragon_tcg_tb_start(DisasContextBase *db, CPUState *cpu) {
    // Do nothing
}

static void dragon_tcg_insn_start(DisasContextBase *db, CPUState *cpu) {
    tcg_gen_insn_start(db->pc_next);
}

static inline int shl_2(DisasContext *ctx, int x) {
    return x << 2;
}

bool decode_insn(DisasContext *ctx, uint32_t insn);
#include "decode-insn.c.inc"

static void dragon_tcg_translation_insn(DisasContextBase *db, CPUState *cpu) {
    DisasContext *ctx = container_of(db, DisasContext, base);
    // pc <= db->pc_next
    tcg_gen_movi_tl(pc, db->pc_next);   

    // Load instruction image
    // translator_ldl(CPUArchState *env, DisasContextBase *db, vaddr pc)
    uint32_t insn = translator_ldl(ctx->env, db, db->pc_next);

    // bool decode_insn(DisasContext *ctx, uint32_t insn)
    bool decoded = decode_insn(ctx, insn);
    if (!decoded) {
        error_report("Illegal instruction, pc: 0x%08lX, inst: 0x%08X\n",
                    db->pc_next, insn);
        // TODO: throw exception
    }
    db->pc_next += 4;
}

static void dragon_tcg_tb_stop(DisasContextBase *db, CPUState *cpu) {
    // DisasContext *ctx = container_of(db, DisasContext, base);
    switch(db->is_jmp) {
        case DISAS_NEXT:
            break;
        case DISAS_TOO_MANY:
            // TODO
            break;
        case DISAS_NORETURN:
            break;
        default:
            g_assert_not_reached();
    }
}

static bool dragon_tcg_disas_log(const DisasContextBase *db, CPUState *cpu, 
                                 FILE *f) {
    return false;
}

static const TranslatorOps dragon_tr_ops = {
    // TODO:
    .init_disas_context = dragon_tcg_init_disas_context,
    .tb_start = dragon_tcg_tb_start,
    .insn_start = dragon_tcg_insn_start,
    .translate_insn = dragon_tcg_translation_insn,
    .tb_stop = dragon_tcg_tb_stop,
    .disas_log = dragon_tcg_disas_log,
};

void dragon_tcg_translate_code(CPUState *cpu, TranslationBlock *tb,
                           int *max_insns, vaddr pc, void *host_pc) {
// void translator_loop(CPUState *cpu, TranslationBlock *tb, int *max_insns,
//                      vaddr pc, void *host_pc, const TranslatorOps *ops,
//                      DisasContextBase *db)
    DisasContext dc = {};
    translator_loop(cpu, tb, max_insns, pc, host_pc, &dragon_tr_ops, &dc.base);                           
}

static bool trans_ADD_W(DisasContext *ctx, arg_ADD_W *a) {
    TCGv_i32 Rd = tcg_temp_new_i32();
    TCGv_i32 Rj = tcg_temp_new_i32();
    TCGv_i32 Rk = tcg_temp_new_i32();
    // tmp = GR[rj][31:0] + GR[rk][31:0]
    tcg_gen_trunc_tl_i32(Rj, cpu_r[a->rj]);
    tcg_gen_trunc_tl_i32(Rk, cpu_r[a->rk]);
    tcg_gen_add_i32(Rd, Rj, Rk);
    // GR[rd] = SignExtend(tmp[31:0],GRLEN)
    tcg_gen_ext_i32_i64(cpu_r[a->rd], Rd);
    return true;
}
static bool trans_ADD_D(DisasContext *ctx, arg_ADD_D *a) {
    TCGv Rd = tcg_temp_new();
    TCGv Rj = cpu_r[a->rj];
    TCGv Rk = cpu_r[a->rk];
    // tmp = GR[rj][63:0] + GR[rk][63:0]
    tcg_gen_add_tl(Rd, Rj, Rk);
    // GR[rd] = tmp[63:0]
    cpu_r[a->rd] = Rd;
    // tcg_gen_add_tl(cpu_r[a->rd], cpu_r[a->rj], cpu_r[a->rk]);
    return true;
}
static bool trans_ADDI_W(DisasContext *ctx, arg_ADDI_W *a) {
    return false;
}
static bool trans_ADDI_D(DisasContext *ctx, arg_ADDI_D *a) {
    return false;
}
static bool trans_LU12I_W(DisasContext *ctx, arg_LU12I_W *a) {
    TCGv Rd = tcg_temp_new();
    TCGv T = tcg_temp_new();
    // GR[rd] = SignExtend({si20, 12'b0}, GRLEN)
    tcg_gen_movi_tl(T, a->si20);
    tcg_gen_shli_tl(Rd, T, 12);
    cpu_r[a->rd] = Rd;
    return true; 
}
static bool trans_LU32I_D(DisasContext *ctx, arg_LU32I_D *a) {
    TCGv Rd = cpu_r[a->rd];
    TCGv T = tcg_temp_new();
    // GR[rd] = {SignExtend(si20, 32), GR[rd][31:0]}
    tcg_gen_movi_tl(T, a->si20);
    tcg_gen_deposit_tl(cpu_r[a->rd], Rd, T, 32, 32);
    return true;
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