// Transforma as palavras do arquivo em tokens
// para facilitar a interpretação do código.
//
// Author: André
// Date: 16/09/2025
//

#pragma once

#ifndef SAP2_COMPILER_TOKENIZER_H
#define SAP2_COMPILER_TOKENIZER_H

#include "../ErrorCodes.h"
#include <stdio.h>
#include <stddef.h>

// O caractere que antecipa todo comentário. O texto para
// de ser considerado comentário uma vez que há uma quebra de linha.
#define COMMENT_CHAR ';'

// Os tipos de token
typedef enum {
   TokenType_Unknown,         // para tokens desconhecidos

   // Hexadecimal
   TokenType_Hexadecimal,     // para valores hexadecimais

   // Palavra Chave e Identificador
   TokenType_Instruction,         // para palavras reservadas (instruções, por exemplo)
   TokenType_Identifier,      // para identificadores
   TokenType_Register,        // para os registradores (Acumulador, B e C)

   // Operadores
   TokenType_Comma,           // para a vírgula
   TokenType_Space,           // para o espaço
   TokenType_Colon,           // para o dois pontos
   TokenType_Semicolon,       // para o ponto e vírgula (comentário)

   // End of File
   TokenType_EOF,             // para o fim do arquivo
} TokenType_t;

// Uma estrutura que representa um token
typedef struct {
   TokenType_t type;
   char* value;
} Token_t;

/**
 * Transforma um texto em tokens
 * @param file o texto a ser transformado
 * @param finalTokenArray o endereço do vetor a ser colocado os tokens
 * @param finalSize o endereço da variável que guardará o tamanho do vetor de tokens
 * @return código de erro
 */
ErrorCode_t tokenize(FILE* file, Token_t** finalTokenArray, size_t* finalSize);

/**
 * Retorna uma string representando o tipo do token
 * @param type tipo do token
 * @return string com o nome do token
 */
char* getTokenTypeString(TokenType_t type);
#endif //SAP2_COMPILER_TOKENIZER_H