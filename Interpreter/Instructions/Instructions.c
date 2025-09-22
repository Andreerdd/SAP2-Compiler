//
//
// Author: André
// Date: 19/09/2025
//

#include "Instructions.h"

#include <string.h>

#define END_OF_INSTRUCTIONS "END_OF_INSTRUCTIONS"

// variável global não é a melhor coisa mas, nesse contexto,
// eu vou abrir uma exceção :)
const char* INSTRUCTIONS[] = {
    "ADD",
    "DCR",
    "HLT",
    "MVI",
    "OUT",
    "SUB",

    // Elemento para marcar o fim do array de comandos. Certifique-se
    // de que ele sempre está no final do array.
    END_OF_INSTRUCTIONS
};

/**
 * Verifica se um dado texto é uma instrução
 * @param str o texto
 * @return se é uma instrução (1) ou não (0)
 */
unsigned char isInstruction(char* str) {
    size_t i = 0;

    while (strcmp(INSTRUCTIONS[i], END_OF_INSTRUCTIONS) != 0) {
        if (strcmp(INSTRUCTIONS[i], str) == 0)
            return 1;
        i++;
    }

    return 0;
}