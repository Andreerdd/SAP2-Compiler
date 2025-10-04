// Interpreta um arquivo na versão de assembly
// do SAP2.
//
// Author: André
// Date: 19/09/2025
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interpreter.h"
#include "environment.h"
#include "Analysis/tokenizer.h"
#include "Analysis/parser.h"
#include "Runtime/evaluate.h"

// Interpreta o arquivo
ErrorCode_t interpret(FILE * file) {

    ErrorCode_t err;

    // Obtem os tokens do arquivo
    Token_t * tokens = NULL;
    size_t tokens_size = 0;

    err = tokenize(file, &tokens, &tokens_size);
    if (err != EXIT_SUCCESS || tokens == NULL || tokens[0].type == TokenType_EOF) {
        // Retorna o erro
        return err;
    }

    // Inicializa o ambiente do SAP2
    Environment env = {
        .memory = calloc(MEMORY_SIZE, sizeof(memoryUnit_t)),
        .programCounter = STARTER_MEMORY_ADDRESS,
        .registers = calloc(NUMBER_OF_REGISTERS, sizeof(hex1_t)),
        .symbolTable = NULL,
        .symbolCount = 0,

        .currentInstruction = 1,
        .usedAddresses = NULL,
        .usedAddressesSize = 0
    };
    env.flags[FLAG_S] = 0;
    // flag de 0 começa inicializado, uma vez que os valores
    // começam zerados
    env.flags[FLAG_Z] = 1;

    // Se não conseguir alocar, retorna um erro
    if (env.memory == NULL || env.registers == NULL) {
        // Libera os tokens
        for (size_t i = 0; i < tokens_size; i++) {
            free(tokens[i].value);
        }
        free(tokens);

        // Libera o ambiente
        free(env.memory);
        free(env.registers);

        // Avisa o usuário e retorna
        RETURN_ERR(EXIT_NO_MEMORY);
    }

    // Chama o parser para entender o código.
    // Aqui, eu poderia usar AST (árvore sintática abstrata),
    // que é uma representação do código. No entanto, como
    // estamos lidando com uma simulação do SAP2, eu preferi
    // fazer essa "representação" na memória.
    parse(tokens, tokens_size, &env);

    // Reinicia o contador de programa
    env.programCounter = STARTER_MEMORY_ADDRESS;
    env.currentInstruction = 1;

    // Avalia(executa) o código
    ErrorCode_t exit_code = evaluate(&env);
    WARN("Saida de Erro: %d\n", exit_code);


    // Fim do código //

    // Libera os tokens
    for (size_t i = 0; i < tokens_size; i++) {
        free(tokens[i].value);
    }
    free(tokens);

    // Libera o ambiente depois das execuções
    free(env.memory);
    free(env.registers);

    // Retorna sucesso
    return EXIT_SUCCESS;
}