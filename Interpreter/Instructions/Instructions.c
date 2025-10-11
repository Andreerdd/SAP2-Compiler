//
//
// Author: André
// Date: 19/09/2025
//

#include "Instructions.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#define END_OF_INSTRUCTIONS "END_OF_INSTRUCTIONS"

// variável global não é a melhor coisa mas, nesse contexto,
// eu vou abrir uma exceção :)
const char* INSTRUCTIONS[] = {
    "ADD",
    "ANA",
    "ANI",
    "CALL",
    "CMA",
    "DCR",
    "HLT",
    "INR",
    "IN",
    "JMP",
    "JM",
    "JNZ",
    "JZ",
    "LDA",
    "MOV",
    "MVI",
    "NOP",
    "ORA",
    "ORI",
    "OUT",
    "RAL",
    "RAR",
    "RET",
    "STA",
    "SUB",
    "XRA",
    "XRI",

    // Elemento para marcar o fim do array de comandos. Certifique-se
    // de que ele sempre está no final do array.
    END_OF_INSTRUCTIONS
};

int compare_str_with_instruction(const char * inst, const char * str) {
    size_t size_inst = strlen(inst), size_str = strlen(str);
    // Se forem de tamanhos diferentes, é falso
    if (size_inst != size_str) return size_inst > size_str ? 1 : -1;

    for (size_t i = 0; i < size_inst; i++) {
        int tli = tolower(inst[i]);
        int tls = tolower(str[i]);
        if (tli != tls) return tli > tls ? 1 : -1;
    }

    // Se chegou até aqui, é igual
    return 0;
}

/**
 * Verifica se um dado texto é uma instrução
 * @param str o texto
 * @return se é uma instrução (1) ou não (0)
 */
unsigned char isInstruction(char* str) {
    size_t i = 0;
    while (strcmp(INSTRUCTIONS[i], END_OF_INSTRUCTIONS) != 0) {
        if (compare_str_with_instruction(INSTRUCTIONS[i], str) == 0)
            return 1;
        i++;
    }

    return 0;
}

/**
 * Retorna o nome da operação do código dado
 * @param opcode Código de Operação da instrução
 * @return nome da instrução
 */
const char* getInstructionName(uhex1_t opcode) {
    switch (opcode) {
        // ADD
        case OPCODE_ADD_B: return "ADD B";
        case OPCODE_ADD_C: return "ADD C";

        // ANA
        case OPCODE_ANA_B: return "ANA B";
        case OPCODE_ANA_C: return "ANA C";

        // ANI
        case OPCODE_ANI: return "ANI";

        // CALL
        case OPCODE_CALL: return "CALL";

        // CMA
        case OPCODE_CMA: return "CMA";

        // DCR
        case OPCODE_DCR_A: return "DCR A";
        case OPCODE_DCR_B: return "DCR B";
        case OPCODE_DCR_C: return "DCR C";

        // HLT
        case OPCODE_HLT: return "HLT";

        // IN
        case OPCODE_IN: return "IN";

        // INR
        case OPCODE_INR_A: return "INR A";
        case OPCODE_INR_B: return "INR B";
        case OPCODE_INR_C: return "INR C";

        // JMP/JM/JNZ/JZ
        case OPCODE_JMP: return "JMP";
        case OPCODE_JM:  return "JM";
        case OPCODE_JNZ: return "JNZ";
        case OPCODE_JZ:  return "JZ";

        // LDA
        case OPCODE_LDA: return "LDA";

        // MOV
        case OPCODE_MOV_A_B: return "MOV A, B";
        case OPCODE_MOV_A_C: return "MOV A, C";
        case OPCODE_MOV_B_A: return "MOV B, A";
        case OPCODE_MOV_B_C: return "MOV B, C";
        case OPCODE_MOV_C_A: return "MOV C, A";
        case OPCODE_MOV_C_B: return "MOV C, B";

        // MVI
        case OPCODE_MVI_A: return "MVI A";
        case OPCODE_MVI_B: return "MVI B";
        case OPCODE_MVI_C: return "MVI C";

        // NOP
        case OPCODE_NOP: return "NOP";

        // ORA/ORI
        case OPCODE_ORA_B: return "ORA B";
        case OPCODE_ORA_C: return "ORA C";
        case OPCODE_ORI:   return "ORI";

        // OUT
        case OPCODE_OUT: return "OUT";

        // RAL/RAR
        case OPCODE_RAL: return "RAL";
        case OPCODE_RAR: return "RAR";

        // RET
        case OPCODE_RET: return "RET";

        // STA
        case OPCODE_STA: return "STA";

        // SUB
        case OPCODE_SUB_B: return "SUB B";
        case OPCODE_SUB_C: return "SUB C";

        // XRA/XRI
        case OPCODE_XRA_B: return "XRA B";
        case OPCODE_XRA_C: return "XRA C";
        case OPCODE_XRI:   return "XRI";

        default:
            return "[Instrucao Sem Nome Definido]"; // Opcode desconhecido
    }
}

unsigned short int getInstructionTStates(uhex1_t opcode) {
    switch (opcode) {
        // DCR
        case OPCODE_DCR_A:
        case OPCODE_DCR_B:
        case OPCODE_DCR_C: return 4;

        // NOP
        case OPCODE_NOP: return 4;

        // JMP
        case OPCODE_JMP: return 10;

        // JM, JN, JNZ
        // Note que aqui, é considerado apenas se não pularem
        case OPCODE_JM:
        case OPCODE_JNZ:
        case OPCODE_JZ:  return 7;

        // MVI
        case OPCODE_MVI_A:
        case OPCODE_MVI_B:
        case OPCODE_MVI_C: return 7;

        // MOV
        case OPCODE_MOV_A_B:
        case OPCODE_MOV_A_C:
        case OPCODE_MOV_B_A:
        case OPCODE_MOV_B_C:
        case OPCODE_MOV_C_A:
        case OPCODE_MOV_C_B: return 4;

        // RET
        case OPCODE_RET: return 10;

        // LDA e STA
        case OPCODE_STA:
        case OPCODE_LDA: return 13;

        // OUT
        case OPCODE_OUT: return 10;

        // CALL
        case OPCODE_CALL: return 18;

        default: return 4; // retorna um valor fixo se não encontrar
    }
}