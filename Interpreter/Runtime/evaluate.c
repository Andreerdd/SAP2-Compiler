//
//
// Author: André
// Date: 21/09/2025
//

#include "evaluate.h"
#include "../Instructions/InstructionsFunctions.h"
#include "../Utils/Utils.h"

// Fetches
#define EVAL_FETCH1()         consume_hex1(env)
#define EVAL_FETCH2()         consume_hex2(env)

// Chamada genérica sem operandos
#define EVAL_CALL0(fn) \
fn(env);

// Chamada genérica com 1 operando já disponível
#define EVAL_CALL1(fn, a1) \
fn(env, (a1));

// Chamada genérica com 2 operandos já disponíveis
#define EVAL_CALL2(fn, a1, a2) \
fn(env, (a1), (a2));

// Padrões de decodificação comuns:
// fn(env) — sem operandos
#define EVAL_ENC__NONE(fn) \
{ EVAL_CALL0(fn);  break; }

// fn(env, reg) — registrador já conhecido (constante/enum)
#define EVAL_ENC__REG(fn, reg) \
{ EVAL_CALL1(fn, (reg));  break; }

// fn(env, reg, reg) — registrador já conhecido (constante/enum)
#define EVAL_ENC__2REG(fn, r1, r2) \
{ EVAL_CALL2(fn, (r1), (r2));  break; }

// fn(env, reg, hex1) — consome 1 byte imediato
#define EVAL_ENC__REG_HEX1(fn, reg) \
{ hex1_t x1 = EVAL_FETCH1(); EVAL_CALL2(fn, (reg), x1);  break; }

// fn(env, hex1) — consome 1 byte imediato
#define EVAL_ENC__HEX1(fn) \
{ hex1_t x1 = EVAL_FETCH1(); EVAL_CALL1(fn, x1); break; }

// fn(env, hex2) — consome 2 bytes imediato (LSB/MSB)
#define EVAL_ENC__HEX2(fn) \
{ hex2_t x2 = EVAL_FETCH2(); EVAL_CALL1(fn, x2); break; }

#define test(o, l) \
case o: l;


/**
 * Consome o próximo valor de memória como um hexadecimal
 * @param env o ambiente do SAP2
 * @return o hexadecimal que leu
 */
hex1_t consume_hex1(Environment * env) {
    return env->memory[env->programCounter++].value;
}

/**
 * Consome o próximo valor de memória como uma instrução (unsigned hex)
 * @param env o ambiente do SAP2
 * @return o hexadecimal que leu
 */
uhex1_t consume_instruction(Environment * env) {
    return env->memory[env->programCounter++].value;
}

/**
 * Consome os dois próximos valores na memória (o primeiro como o
 * LSB, byte menos significante, e o segundo como o MSB, byte mais
 * significante) e retorna um hexadecimal formado por estes.
 * @param env o ambiente do SAP2
 * @return o hexadecimal formado pelos dois valores
 */
hex2_t consume_hex2(Environment * env) {
    hex1_t lsb = consume_hex1(env);
    hex1_t msb = consume_hex1(env);
    return (hex2_t)((msb << 8) | (lsb & 0xFF));
}



ErrorCode_t execute_instruction(Environment * env) {
    env->last_instruction = env->memory[env->programCounter];
    uhex1_t opcode = consume_hex1(env);

    switch (opcode) {
        // ADD
        case OPCODE_ADD_B: EVAL_ENC__REG(execute_add, REGISTER_B)
        case OPCODE_ADD_C: EVAL_ENC__REG(execute_add, REGISTER_C)

        // ANA
        case OPCODE_ANA_B: EVAL_ENC__REG(execute_ana, REGISTER_B)
        case OPCODE_ANA_C: EVAL_ENC__REG(execute_ana, REGISTER_C)

        // ANI
        case OPCODE_ANI:   EVAL_ENC__HEX1(execute_ani)

        // CALL
        case OPCODE_CALL:  EVAL_ENC__HEX2(execute_call)

        // CMA
        case OPCODE_CMA:   EVAL_ENC__NONE(execute_cma)

        // DCR
        case OPCODE_DCR_A: EVAL_ENC__REG(execute_dcr, ACCUMULATOR);
        case OPCODE_DCR_B: EVAL_ENC__REG(execute_dcr, REGISTER_B);
        case OPCODE_DCR_C: EVAL_ENC__REG(execute_dcr, REGISTER_C);

        // HLT
        case OPCODE_HLT: {
            return execute_hlt(env);
        }

        // IN
        case OPCODE_IN: EVAL_ENC__HEX2(execute_out)

        // INR
        case OPCODE_INR_A: EVAL_ENC__REG(execute_inr, ACCUMULATOR)
        case OPCODE_INR_B: EVAL_ENC__REG(execute_inr, REGISTER_B)
        case OPCODE_INR_C: EVAL_ENC__REG(execute_inr, REGISTER_C)

        // JMP/JM/JNZ/JZ
        case OPCODE_JMP: EVAL_ENC__HEX2(execute_jmp)
        case OPCODE_JM:  EVAL_ENC__HEX2(execute_jm)
        case OPCODE_JNZ: EVAL_ENC__HEX2(execute_jnz)
        case OPCODE_JZ:  EVAL_ENC__HEX2(execute_jz)

        // LDA
        case OPCODE_LDA: EVAL_ENC__HEX2(execute_lda)

        // MOV
        case OPCODE_MOV_A_B: EVAL_ENC__2REG(execute_mov, ACCUMULATOR, REGISTER_B)
        case OPCODE_MOV_A_C: EVAL_ENC__2REG(execute_mov, ACCUMULATOR, REGISTER_C)
        case OPCODE_MOV_B_A: EVAL_ENC__2REG(execute_mov, REGISTER_B, ACCUMULATOR)
        case OPCODE_MOV_B_C: EVAL_ENC__2REG(execute_mov, REGISTER_B, REGISTER_C)
        case OPCODE_MOV_C_A: EVAL_ENC__2REG(execute_mov, REGISTER_C, ACCUMULATOR)
        case OPCODE_MOV_C_B: EVAL_ENC__2REG(execute_mov, REGISTER_C, REGISTER_B)

        // MVI
        case OPCODE_MVI_A: EVAL_ENC__REG_HEX1(execute_mvi, ACCUMULATOR)
        case OPCODE_MVI_B: EVAL_ENC__REG_HEX1(execute_mvi, REGISTER_B)
        case OPCODE_MVI_C: EVAL_ENC__REG_HEX1(execute_mvi, REGISTER_C)

        // NOP
        case OPCODE_NOP: EVAL_ENC__NONE(execute_nop)

        // ORA/ORI
        case OPCODE_ORA_B: EVAL_ENC__REG(execute_ora, REGISTER_B)
        case OPCODE_ORA_C: EVAL_ENC__REG(execute_ora, REGISTER_C)
        case OPCODE_ORI:   EVAL_ENC__HEX1(execute_ori)

        // OUT
        case OPCODE_OUT: EVAL_ENC__HEX2(execute_out)

        // RAL/RAR
        case OPCODE_RAL: EVAL_ENC__NONE(execute_ral)
        case OPCODE_RAR: EVAL_ENC__NONE(execute_rar)

        // RET
        case OPCODE_RET: EVAL_ENC__NONE(execute_ret)

        // STA
        case OPCODE_STA: EVAL_ENC__HEX2(execute_sta)

        // SUB
        case OPCODE_SUB_B: EVAL_ENC__REG(execute_sub, REGISTER_B)
        case OPCODE_SUB_C: EVAL_ENC__REG(execute_sub, REGISTER_C)

        // XRA/XRI
        case OPCODE_XRA_B: EVAL_ENC__REG(execute_xra, REGISTER_B)
        case OPCODE_XRA_C: EVAL_ENC__REG(execute_xra, REGISTER_C)
        case OPCODE_XRI:   EVAL_ENC__HEX1(execute_xri)

        default: {
            fprintf(stderr,"Instrucao %d: Codigo de Operacao \"%x\" desconhecido", env->currentInstruction, opcode);
            return EXIT_INVALID_INSTRUCTION;
        }
    }

    env->currentInstruction++;

    return EXIT_SUCCESS;
}

// Imprime as informações e espera o usuário apertar enter
void debugIfOn(Environment * env) {
    if (env_params->debug_mode) {
        // Para o print abaixo, eu poderia ter só voltado na memória
        // até achar algo com uma anotação. No entanto, assim é mais
        // bonitinho :)
        printf("Instrucao atual: %s\n", env->last_instruction.annotation);

        // Se a última instrução for alguma específica, trata ela de forma diferente
        uhex1_t liv = env->last_instruction.value;
        if (liv == OPCODE_OUT) {
            print_hex(__out_flow(env->hex_flow_buffer), env->registers[ACCUMULATOR]);
            printf("Saida atual: ");
            printf(">> Pressione enter para continuar");
            enter_to_continue();
        } else if (liv == OPCODE_IN) {
            setRegister(env, ACCUMULATOR, get_1hex_from_in(env->hex_flow_buffer));
            printf("\n");
        }

        print_debug_info(env);

        printf(">> Pressione enter para continuar");
        enter_to_continue();
        printf("\n\n\n\n");
    }
}

ErrorCode_t evaluate(Environment * env) {
    ErrorCode_t err;
    while (env->programCounter < MEMORY_SIZE) {
        if (env_params->max_evaluated != -1 && env->currentInstruction > env_params->max_evaluated) {
            V_EXIT(
                EXIT_LIMIT_REACHED,
                "Instrucao %i: nao foi possivel executar essa instrucao\nporque o programa atingiu o limite de execucao.",
                env->currentInstruction);
        }

        err = execute_instruction(env);
        if (err != EXIT_SUCCESS) {
            if (err == EXIT_HLT) // nesse caso, HLT é SUCESSO
                return EXIT_SUCCESS;
            debugIfOn(env);
            return err;
        }
        debugIfOn(env);
    }
    return EXIT_SUCCESS;
}