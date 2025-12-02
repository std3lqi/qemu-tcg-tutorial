#include "exec/helper-proto.h"
DEF_HELPER_1(raise_illegal_instruction, noreturn, env)
DEF_HELPER_3(bitrev_w, void, env, i32, i32)
DEF_HELPER_3(bitrev_d, void, env, i32, i32)