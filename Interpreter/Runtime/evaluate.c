//
//
// Author: André
// Date: 21/09/2025
//

#include "evaluate.h"
#include "../Instructions/InstructionsFunctions.h"

// Fetches
#define EVAL_FETCH1()         consume_hex1(env)
#define EVAL_FETCH2()         consume_hex2(env)

// Chamada genérica sem operandos
#define EVAL_CALL0(fn) \
do { fn(env); } while (0); break;

// Chamada genérica com 1 operando já disponível
#define EVAL_CALL1(fn, a1) \
do { fn(env, (a1)); } while (0); break;

// Chamada genérica com 2 operandos já disponíveis
#define EVAL_CALL2(fn, a1, a2) \
do { fn(env, (a1), (a2)); } while (0); break;

// Padrões de decodificação comuns:
// fn(env) — sem operandos
#define EVAL_ENC__NONE(fn) \
do { EVAL_CALL0(fn); } while (0); break;

// fn(env, reg) — registrador já conhecido (constante/enum)
#define EVAL_ENC__REG(fn, reg) \
do { EVAL_CALL1(fn, (reg)); } while (0); break;

// fn(env, reg, hex1) — consome 1 byte imediato
#define EVAL_ENC__REG_HEX1(fn, reg) \
do { uhex1_t x1 = EVAL_FETCH1(); EVAL_CALL2(fn, (reg), x1); } while (0); break;

// fn(env, hex1) — consome 1 byte imediato
#define EVAL_ENC__HEX1(fn) \
do { uhex1_t x1 = EVAL_FETCH1(); EVAL_CALL1(fn, x1); } while (0); break;

// fn(env, hex2) — consome 2 bytes imediato (LSB/MSB)
#define EVAL_ENC__HEX2(fn) \
do { uhex2_t x2 = EVAL_FETCH2(); EVAL_CALL1(fn, x2); } while (0); break;

#define test(o, l) \
case o: l;


/**
 * Consome o próximo valor de memória como um hexadecimal
 * @param env o ambiente do SAP2
 * @return o hexadecimal que leu
 */
uhex1_t consume_hex1(Environment * env) {
    return env->memory[env->programCounter++];
}

/**
 * Consome os dois próximos valores na memória (o primeiro como o
 * LSB, byte menos significante, e o segundo como o MSB, byte mais
 * significante) e retorna um hexadecimal formado por estes.
 * @param env o ambiente do SAP2
 * @return o hexadecimal formado pelos dois valores
 */
uhex2_t consume_hex2(Environment * env) {
    uhex1_t lsb = consume_hex1(env);
    uhex1_t msb = consume_hex1(env);
    return (msb << 8) | lsb;
}



ErrorCode_t execute_instruction(Environment * env) {
    uhex2_t opcode = consume_hex1(env);

    switch (opcode) {
        // ADD
        case OPCODE_ADD_B: EVAL_ENC__REG(execute_add, REGISTER_B)
        case OPCODE_ADD_C: EVAL_ENC__REG(execute_add, REGISTER_C)

        // DCR
        case OPCODE_DCR_A: EVAL_ENC__REG(execute_dcr, ACCUMULATOR);
        case OPCODE_DCR_B: EVAL_ENC__REG(execute_dcr, REGISTER_B);
        case OPCODE_DCR_C: EVAL_ENC__REG(execute_dcr, REGISTER_C);

        // HLT
        case OPCODE_HLT: {
            EVAL_ENC__NONE(execute_hlt)
            return EXIT_HLT; // não precisa
        }

        // MVI
        case OPCODE_MVI_A: EVAL_ENC__REG_HEX1(execute_mvi, ACCUMULATOR)
        case OPCODE_MVI_B: EVAL_ENC__REG_HEX1(execute_mvi, REGISTER_B)
        case OPCODE_MVI_C: EVAL_ENC__REG_HEX1(execute_mvi, REGISTER_C)

        // OUT
        case OPCODE_OUT: EVAL_ENC__HEX2(execute_out)

        // SUB
        case OPCODE_SUB_B: EVAL_ENC__REG(execute_sub, REGISTER_B)
        case OPCODE_SUB_C: EVAL_ENC__REG(execute_sub, REGISTER_C)

        default: {
            WARN("Instrucao %d: Codigo de Operacao \"%x\" desconhecido", env->currentInstruction, opcode);
            return EXIT_INVALID_INSTRUCTION;
        }
    }

    env->currentInstruction++;

    return EXIT_SUCCESS;
}

ErrorCode_t evaluate(Environment * env) {
    ErrorCode_t err;
    while (env->programCounter < MEMORY_SIZE) {
        err = execute_instruction(env);
        if (err != EXIT_SUCCESS) return err;
    }
    return EXIT_SUCCESS;
}