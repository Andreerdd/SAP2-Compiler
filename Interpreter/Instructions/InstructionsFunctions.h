// Funções de cada instrução
//
// Author: André
// Date: 19/09/2025
//

#ifndef SAP2_COMPILER_INSTRUCTIONSFUNCTIONS_H
#define SAP2_COMPILER_INSTRUCTIONSFUNCTIONS_H

#include <stdint.h>

#include "../environment.h"
#include "Instructions.h"
#include "../ErrorCodes.h"
#define MAX_OPCODE 0xFF

#define SHOW_MEMORY_ON_HLT (1)

// instrução que só recebe o ambiente
#define ex_fn(name) \
    ErrorCode_t name(Environment* env)

// instrução que recebe o ambiente e um registrador
#define ex_fn_reg(name) \
    ErrorCode_t name(Environment* env, hex1_t reg)

// instrução que recebe o ambiente e um valor
#define ex_fn_val(name) \
    ErrorCode_t name(Environment* env, hex2_t value)

// instrução que recebe um registrador e um valor
#define ex_fn_rv(name) \
    ErrorCode_t name(Environment* env, hex1_t reg, hex1_t value)

// instrução que recebe dois registradores
#define ex_fn_2reg(name) \
    ErrorCode_t name(Environment* env, hex1_t ri, hex1_t rj)

// instrução que recebe um valor hex2
#define ex_fn_hex2(name) \
    ErrorCode_t name(Environment*env, hex2_t value)

ex_fn_reg(execute_add);
ex_fn_reg(execute_dcr);
ex_fn(execute_hlt);
ex_fn_hex2(execute_jmp);
ex_fn_rv(execute_mvi);
ex_fn_val(execute_out);
ex_fn_reg(execute_sub);

ex_fn_reg(execute_ana);
ex_fn_val(execute_ani);
ex_fn_hex2(execute_call);
ex_fn(execute_cma);
ex_fn_reg(execute_inr);
ex_fn_hex2(execute_jm);
ex_fn_hex2(execute_jnz);
ex_fn_hex2(execute_jz);
ex_fn_hex2(execute_lda);
ex_fn_2reg(execute_mov);
ex_fn(execute_nop);
ex_fn_reg(execute_ora);
ErrorCode_t execute_ori(Environment* env, hex1_t value);
ex_fn(execute_ral);
ex_fn(execute_rar);
ex_fn(execute_ret);
ex_fn_hex2(execute_sta);
ex_fn_reg(execute_xra);
ErrorCode_t execute_xri(Environment* env, hex1_t value);



extern Instruction_t INSTRUCTION_SET[MAX_OPCODE];

#endif //SAP2_COMPILER_INSTRUCTIONSFUNCTIONS_H