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
#define MAX_OPCODE 0xFF

#define SHOW_MEMORY_ON_HLT (1)

// instrução que só recebe o ambiente
#define ex_fn(name) \
    void name(Environment* env)

// instrução que recebe o ambiente e um registrador
#define ex_fn_reg(name) \
    void name(Environment* env, uhex1_t reg)

// instrução que recebe o ambiente e um valor
#define ex_fn_val(name) \
    void name(Environment* env, uhex2_t value)

// instrução que recebe um registrador e um valor
#define ex_fn_rv(name) \
    void name(Environment* env, uhex1_t reg, uhex1_t value)

// instrução que recebe dois registradores
#define ex_fn_2reg(name) \
    void name(Environment* env, uhex1_t ri, uhex1_t rj)

ex_fn_reg(execute_add);
ex_fn_reg(execute_dcr);
ex_fn(execute_hlt);
ex_fn_rv(execute_mvi);
ex_fn_val(execute_out);
ex_fn_reg(execute_sub);


extern Instruction_t INSTRUCTION_SET[MAX_OPCODE];

#endif //SAP2_COMPILER_INSTRUCTIONSFUNCTIONS_H