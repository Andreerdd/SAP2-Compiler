// Códigos de erro e mensagens usadas no programa
//
// Author: André
// Date: 16/09/2025
//
#pragma once

#ifndef SAP2_COMPILER_ERRORCODES_H
#define SAP2_COMPILER_ERRORCODES_H

#include <stdio.h>
#include <stdlib.h>

// Os códigos de erro //
typedef enum {
    EXIT_NO_MEMORY = -1,                // se não houver memória
    // EXIT_SUCCESS = 0,                // sucesso (já definido no <stdlib.h>)
    EXIT_NO_FILE = 1,                   // se não foi dado um arquivo na execução do código
    EXIT_FILE_NOT_FOUND = 2,            // se o arquivo dado não foi encontrado
    EXIT_INVALID_ARGUMENT = 3,          // se o argumento dado for inválido
    EXIT_NULL_ARGUMENT = 4,             // se o argumento for nulo
    EXIT_INVALID_INSTRUCTION = 5,       // se a instrução dada for inválida
    EXIT_READ_ONLY_ADDRESS = 6,         // se a posição de memória for READ-ONLY mas o usuário tentar editar
    EXIT_HLT = 7,                       // quando a instrução HLT é executada
    EXIT_INSTRUCTION_LIMIT_REACHED = 8, // quando o programa alcança o limite de instruções executadas
    EXIT_TIME_LIMIT_REACHED = 9,        // quando o programa alcança o limite de tempo
    EXIT_ILLEGAL_HEX = 10               // quando é tentado colocar um hexadecimal de tamanho maior que aguenta
} ErrorCode_t;

// As mensagens de erro //
#define EXIT_NO_MEMORY_MESSAGE "Nao ha memoria disponivel para a interpretacao e/ou execucao do programa."
#define EXIT_NO_FILE_MESSAGE "Uso incorreto do comando!\nComo usar o comando:\n\t./sap2 <arquivo.asm> ..."
#define EXIT_FILE_NOT_FOUND_MESSAGE "O arquivo dado nao foi encontrado!\nCertifique-se, por exemplo, de estar considerando o diretorio do arquivo."
#define EXIT_INVALID_ARGUMENT_MESSAGE "Argumento Invalido"
#define EXIT_INVALID_INSTRUCTION_MESSAGE "Instrucao Invalida"
#define EXIT_ILLEGAL_HEX_MESSAGE "Numero hexadecimal excedeu o limite aceito"

// Macros //

// Imprime a respectiva mensagem de erro e retorna o código de erro //
#define RETURN_ERR(E) do {                      \
    fprintf(stderr, "Erro: %s\n", E##_MESSAGE); \
    return (E);                                 \
} while (0)
// Com mensagem customizada
#define RETURN_CUSTOM_ERR(E, MSG) do {  \
    fprintf(stderr, "Erro: %s\n", MSG); \
    return (E);                         \
} while (0)

// Imprime a respectiva mensagem de erro e sai do programa //
#define EXIT_ERR(E) do {                                \
    fprintf(stderr, "Erro Interno: %s\n", E##_MESSAGE); \
    exit(E);                                            \
} while (0)
// Com mensagem customizada
#define EXIT_CUSTOM_ERR(E, MSG) do {            \
    fprintf(stderr, "Erro Interno: %s\n", MSG); \
    exit(E);                                    \
} while (0)
// Com variadic args (estilo printf)
#define V_EXIT(E, format, ...) do { \
    fprintf(stderr, "[ERRO] ");  \
    fprintf(stderr, format, __VA_ARGS__); fprintf(stdout, "\n");   \
    exit(E);                                                       \
} while (0)
// Mostra em que instrução está
#define I_EXIT(E) {                                                                              \
            fprintf(stderr, "[ERRO] Instrucao %d: %s\n", state->env.currentInstruction, E##_MESSAGE);   \
            exit(E);                                                                             \
} while (0)
// Com variadic args (estilo printf) e mostra em que instrução está
#define VI_EXIT(E, format, ...) do {                                                                \
    fprintf(stderr, "[ERRO] Instrucao %d: %s\n\t", state->env->currentInstruction, E##_MESSAGE);    \
    fprintf(stderr, format, __VA_ARGS__); fprintf(stdout, "\n");                                    \
    exit(E);                                                                                        \
} while (0)

// Imprime uma mensagem de aviso ao usuário //
#define WARN(format, ...) do {              \
    fprintf(stdout, "\n[AVISO] ");   \
    fprintf(stdout, format, __VA_ARGS__);   \
    fprintf(stdout, "\n");                  \
    } while(0)
// Mostra em que instrução está
#define I_WARN(format, ...) {                                                           \
    fprintf(stdout, "[AVISO] Instrucao %d:\n\t", state->env->currentInstruction);       \
    fprintf(stdout, format, __VA_ARGS__);                                               \
    fprintf(stdout, "\n");                                                              \
} while (0)


#endif //SAP2_COMPILER_ERRORCODES_H