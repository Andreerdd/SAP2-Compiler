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

    // DCR
    OPCODE_DCR_A = 0x3D,
    OPCODE_DCR_B = 0x05,
    OPCODE_DCR_C = 0x0D,

    // MVI
    OPCODE_MVI_A = 0x3E,
    OPCODE_MVI_B = 0x06,
    OPCODE_MVI_C = 0x0E,

    // HLT
    OPCODE_HLT = 0x76,

    // OUT
    OPCODE_OUT = 0xD3,

    // SUB
    OPCODE_SUB_B = 0x90,
    OPCODE_SUB_C = 0x91,
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

#endif //SAP2_COMPILER_INSTRUCTIONS_H