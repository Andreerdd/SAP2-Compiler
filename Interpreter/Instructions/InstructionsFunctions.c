//
//
// Author: André
// Date: 19/09/2025
//

#include <stdio.h>
#include <stdlib.h>

#include "InstructionsFunctions.h"
#include "../Utils/Utils.h"
#include "../ErrorCodes.h"
#include "../environment.h"

// env->memory[address]
#define env_memory(address) env->memory[address]

// env->memory[address].value
#define env_memval(address) env_memory(address).value

// definir o registrador
#define SET_REG(r, v) setRegister(env, r, v)

// definir o acumulador
#define SET_ACC(v) SET_REG(ACCUMULATOR, v)

// env->registers[r]
#define REG(r) env->registers[r]

// env->registers[ACCUMULATOR]
#define REG_A REG(ACCUMULATOR)

ex_fn_reg(execute_add) {
    SET_ACC(REG_A + env->registers[reg]);
    return EXIT_SUCCESS;
}

ex_fn_reg(execute_dcr) {
    SET_REG(reg, REG(reg)-1);
    return EXIT_SUCCESS;
}

ex_fn(execute_hlt) {
    fflush(stdout);
    // Antes de sair, imprime a memória

    // Esse if é praticamente desnecessário, já que, para chegar aqui,
    // precisaria de ter o OPCODE do HLT na memória (portanto, algum
    // endereço da memória foi usado).
    if (env->usedAddressesSize > 0 && SHOW_MEMORY_ON_HLT) {
        printf("\nMemoria RAM ================================\nEndereco\t|  Conteudo\t|  Simbolico\n");

        qsort(env->usedAddresses, env->usedAddressesSize, sizeof(hex2_t), comp_hex2);

        for (size_t i = 0; i < env->usedAddressesSize; i++) {
            char* annotation = env_memory(env->usedAddresses[i]).annotation;
            if (annotation != NULL) {
                // é instrução
                printf("%xH\t\t|  %xH \t\t|  %s\n",
                env->usedAddresses[i],
                // Se for instrução, o valor guardado deverá ser lido como
                // unsigned hex.
                (uhex1_t)env_memval(env->usedAddresses[i]),
                env_memory(env->usedAddresses[i]).annotation);

            } else {
                printf("%xH\t\t|  %xH \t\t|  %s\n",
                env->usedAddresses[i],
                // Se não for instrução, deverá ser lido como signed hex.
                env_memval(env->usedAddresses[i]),
                env_memory(env->usedAddresses[i]).annotation);
            }
        }
    }

    //exit(EXIT_SUCCESS);
    return EXIT_HLT;
}

ex_fn_hex2(execute_jmp) {
    env->programCounter = value;
    return EXIT_SUCCESS;
}

ex_fn_rv(execute_mvi) {
    SET_REG(reg, value);
    return EXIT_SUCCESS;
}

ex_fn_val(execute_out) {
    printf("%xH (Decimal: %d)\n", (uhex1_t)REG_A, REG_A);
    fflush(stdout);
    return EXIT_SUCCESS;
}

ex_fn_reg(execute_sub) {
    SET_ACC(REG_A - REG(reg));
    return EXIT_SUCCESS;
}

// ANA reg: A = A & R
ex_fn_reg(execute_ana) {
    SET_ACC(REG_A & REG(reg));
    return EXIT_SUCCESS;
}

// ANI byte: A = A & byte
ex_fn_val(execute_ani) {
    SET_ACC(REG_A & value);
    return EXIT_SUCCESS;
}

// CMA: A = ~A
ex_fn(execute_cma) {
    SET_ACC((hex1_t)(~REG_A));
    return EXIT_SUCCESS;
}

// INR reg: R = R + 1
ex_fn_reg(execute_inr) {
    SET_REG(reg, (hex1_t)(REG(reg) + 1));
    return EXIT_SUCCESS;
}

// JM addr: if S==1 PC=addr
ex_fn_hex2(execute_jm) {
    if (env->flags[FLAG_S]) {
        env->programCounter = value;
    }
    return EXIT_SUCCESS;
}

// JNZ addr: if Z==0 PC=addr
ex_fn_hex2(execute_jnz) {
    if (!env->flags[FLAG_Z]) {
        env->programCounter = value;
    }
    return EXIT_SUCCESS;
}

// JZ addr: if Z==1 PC=addr
ex_fn_hex2(execute_jz) {
    if (env->flags[FLAG_Z]) {
        env->programCounter = value;
    }
    return EXIT_SUCCESS;
}

// LDA addr: A = M[addr]
ex_fn_hex2(execute_lda) {
    SET_ACC(env_memval(value));
    return EXIT_SUCCESS;
}

// MOV ri, rj: R_i = R_j
ex_fn_2reg(execute_mov) {
    SET_REG(ri, REG(rj));
    return EXIT_SUCCESS;
}

// NOP: faz nada
ex_fn(execute_nop) {
    return EXIT_SUCCESS;
}

// ORA reg: A = A | R
ex_fn_reg(execute_ora) {
    SET_ACC(REG_A | REG(reg));
    return EXIT_SUCCESS;
}

// ORI byte: A = A | byte
ErrorCode_t execute_ori(Environment* env, hex1_t value) {
    SET_ACC(REG_A | value);
    return EXIT_SUCCESS;
}

// RAL: A = (A << 1), LSB := 0 (simples)
ex_fn(execute_ral) {
    SET_ACC((hex1_t)((REG_A << 1) & 0xFE));
    return EXIT_SUCCESS;
}

// RAR: A = (A >> 1), MSB -> LSB (simples)
ex_fn(execute_rar) {
    hex1_t msb = (REG_A & 0x80) ? 1 : 0;
    //REG_A = (hex1_t)((REG_A >> 1) | (msb ? 0x80 : 0x00)); // descrição diz MSB vai para LSB
    // Exatamente "MSB vai para LSB":
     REG_A = (hex1_t)((REG_A & 0x7F) | (msb ? 0x01 : 0x00));
    return EXIT_SUCCESS;
}

// RET: PC = M[RET_ADDRESS] (endereço de retorno salvo previamente)
ex_fn(execute_ret) {
    hex1_t lsb = env_memval(RET_ADDRESS_LSB);
    hex1_t msb = env_memval(RET_ADDRESS_MSB);

    env->programCounter = (uhex2_t)(((msb & 0xFF) << 8) | (lsb & 0xFF));

    // Se seu retorno é 2 bytes, ajuste para ler dois bytes (LSB/MSB).
    return EXIT_SUCCESS;
}

// STA addr: M[addr] = A
ex_fn_hex2(execute_sta) {
    env_memval(value) = REG_A;
    return EXIT_SUCCESS;
}

// XRA reg: A = A ^ R
ex_fn_reg(execute_xra) {
    SET_ACC(REG_A ^ REG(reg));
    return EXIT_SUCCESS;
}

// XRI byte: A = A ^ byte
ErrorCode_t execute_xri(Environment* env, hex1_t value) {
    SET_ACC(REG_A ^ value);
    return EXIT_SUCCESS;
}
