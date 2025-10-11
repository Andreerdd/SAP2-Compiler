// Lista das instruções
//
// Author: André
// Date: 19/09/2025
//

#ifndef SAP2_COMPILER_INSTRUCTIONS_H
#define SAP2_COMPILER_INSTRUCTIONS_H

#include "../environment.h"

// Instruções
extern const char* INSTRUCTIONS[];

// OpCodes
typedef enum {
    // ADD
    OPCODE_ADD_B = 0x80,
    OPCODE_ADD_C = 0x81,

    // ANA
    OPCODE_ANA_B = 0xA0,
    OPCODE_ANA_C = 0xA1,

    // ANI
    OPCODE_ANI = 0xE6,

    // CALL
    OPCODE_CALL = 0xCD,

    // CMA
    OPCODE_CMA = 0x2F,

    // DCR
    OPCODE_DCR_A = 0x3D,
    OPCODE_DCR_B = 0x05,
    OPCODE_DCR_C = 0x0D,

    // HLT
    OPCODE_HLT = 0x76,

    // IN
    OPCODE_IN = 0xDB,

    // INR
    OPCODE_INR_A = 0x3C,
    OPCODE_INR_B = 0x04,
    OPCODE_INR_C = 0x0C,

    // JMP/JM/JNZ/JZ
    OPCODE_JMP = 0xC3,
    OPCODE_JM  = 0xFA,
    OPCODE_JNZ = 0xC2,
    OPCODE_JZ  = 0xCA,

    // LDA
    OPCODE_LDA = 0x3A,

    // MOV
    OPCODE_MOV_A_B = 0x78,
    OPCODE_MOV_A_C = 0x79,
    OPCODE_MOV_B_A = 0x47,
    OPCODE_MOV_B_C = 0x41,
    OPCODE_MOV_C_A = 0x4F,
    OPCODE_MOV_C_B = 0x48,

    // MVI
    OPCODE_MVI_A = 0x3E,
    OPCODE_MVI_B = 0x06,
    OPCODE_MVI_C = 0x0E,

    // NOP
    OPCODE_NOP = 0x00,

    // ORA/ORI
    OPCODE_ORA_B = 0xB0,
    OPCODE_ORA_C = 0xB1,
    OPCODE_ORI   = 0xF6,

    // OUT
    OPCODE_OUT = 0xD3,

    // RAL/RAR
    OPCODE_RAL = 0x17,
    OPCODE_RAR = 0x1F,

    // RET
    OPCODE_RET = 0xC9,

    // STA
    OPCODE_STA = 0x32,

    // SUB
    OPCODE_SUB_B = 0x90,
    OPCODE_SUB_C = 0x91,

    // XRA/XRI
    OPCODE_XRA_B = 0xA8,
    OPCODE_XRA_C = 0xA9,
    OPCODE_XRI   = 0xEE,
} Opcode_t;

// Estrutura que representa uma instrução
typedef struct {
    Opcode_t opcode;
    void (*execute)(Environment * env, ...);
} Instruction_t;

/**
 * Verifica se um dado texto é uma instrução
 * @param str o texto
 * @return se é uma instrução (1) ou não (0)
 */
unsigned char isInstruction(char* str);

/**
 * Retorna o nome da operação do código dado
 * @param opcode Código de Operação da instrução
 * @return nome da instrução
 */
const char* getInstructionName(uhex1_t opcode);

/**
 * Retorna a quantidade de T States que a instrução dada usa
 * @param opcode o código da instrução
 * @return a quantidade de T States que a instrução usa
 */
unsigned short int getInstructionTStates(uhex1_t opcode);

#endif //SAP2_COMPILER_INSTRUCTIONS_H