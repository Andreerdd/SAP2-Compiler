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

// acessa o registrador dado
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
    // Tira do buffer e coloca na saida
    fflush(stdout);

    // Antes de sair, imprime a memória

    // A verificação de "env->usedAddressesSize > 0" é praticamente desnecessária,
    // já que, para chegar aqui, precisaria de ter o OPCODE do HLT na memória
    // (portanto, algum endereço da memória foi usado).
    if (env->usedAddressesSize > 0 && env_params->hlt_prints_memory) {
        print_info(env);
    }

    return EXIT_HLT;
}

ex_fn_hex2(execute_jmp) {
    env->programCounter = value;
    return EXIT_SUCCESS;
}

ex_fn_hex2(execute_call) {
    // Verifica se já tem coisa escrita

    if (env_memval(RET_ADDRESS_LSB) != 0 || env_memval(RET_ADDRESS_MSB) != 0) {
        WARN(
"Instrucao %d: A instrucao \"CALL\" foi chamada durante uma subrotina.\nPor causa disso, o endereco de memoria para retornar (RET) foi sobrescrito.\nIsso nao eh recomendado, uma vez que seu programa pode \"se perder\".",
            env->currentInstruction);
    }

    hex1_t lsb = (hex1_t) env->programCounter & 0xFF;
    hex1_t msb = (hex1_t) (env->programCounter >> 8) & 0xFF;

    // eu poderia só fazer: setMemoryHex2(env, RET_ADDRESS_LSB, env->programCounter);
    setMemory(env, RET_ADDRESS_LSB, lsb);
    setMemory(env, RET_ADDRESS_MSB, msb);

    env->programCounter = value;
    return EXIT_SUCCESS;
}

ex_fn_rv(execute_mvi) {
    SET_REG(reg, value);
    return EXIT_SUCCESS;
}

ex_fn_val(execute_out) {
    if (!env_params->debug_mode) {
        print_hex(stdoutflow, REG_A);
        fflush(stdoutflow);
    } else {
        env->hex_print_buffer = REG_A;
    }
    return EXIT_SUCCESS;
}

ex_fn_val(execute_in) {
    if (!env_params->debug_mode) {
        SET_ACC(get_1hex_from_in(value));
    } else {
        env->hex_flow_buffer = value;
    }
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
    uhex1_t msb = (REG_A & 0x80) >> 7; // pega o bit 7 e coloca na posição 0
    uhex1_t result = ((REG_A << 1) | msb) & 0xFF;
    SET_ACC((hex1_t)result);
    return EXIT_SUCCESS;
}

// RAR: A = (A >> 1), MSB -> LSB (simples)
ex_fn(execute_rar) {
    hex1_t msb = (REG_A & 0x80) ? 1 : 0;
    //REG_A = (hex1_t)((REG_A >> 1) | (msb ? 0x80 : 0x00)); // descrição diz MSB vai para LSB
    // Exatamente "MSB vai para LSB":
    SET_ACC(REG_A >> 1);
    return EXIT_SUCCESS;
}

// RET: PC = M[RET_ADDRESS] (endereço de retorno salvo previamente)
ex_fn(execute_ret) {
    hex1_t lsb = env_memval(RET_ADDRESS_LSB);
    hex1_t msb = env_memval(RET_ADDRESS_MSB);

    // É seguro ver se ambos são 0 para ver se
    // aponta para um lugar.
    if (lsb == 0 && msb == 0) {
        WARN(
"Instrucao %d: a instrucao RET foi chamada mesmo nao havendo\nnada salvo nos enderecos de memoria de retorno.", env->currentInstruction);
    }

    env->programCounter = (uhex2_t)(((msb & 0xFF) << 8) | (lsb & 0xFF));

    // Esvazia os endereços de retorno (para fins de debug)
    setMemory(env, RET_ADDRESS_MSB, 0);
    setMemory(env, RET_ADDRESS_LSB, 0);

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
