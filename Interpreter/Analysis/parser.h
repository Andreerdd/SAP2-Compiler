// Analisa e avalia as instruções que o usuário deu.
//
// Author: André
// Date: 19/09/2025
//

#ifndef SAP2_COMPILER_PARSER_H
#define SAP2_COMPILER_PARSER_H

#include "tokenizer.h"
#include "../environment.h"

/**
 * Analisa as instruções que o usuário passou.
 * @param tokens array dos tokens das instruções
 * @param size tamanho do array de tokens
 * @param env ambiente do sap2
 */
void parse(Token_t * tokens, size_t size, Environment * env);

#endif //SAP2_COMPILER_PARSER_H