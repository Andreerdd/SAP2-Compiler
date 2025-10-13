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
#include "Utils/Utils.h"

// Retorna as configurações de parâmetros normais
Parametros * get_standard_parameters() {
    Parametros * params = malloc(sizeof(Parametros));

    params->start_address = STANDARD_STARTER_MEMORY_ADDRESS;
    params->hlt_prints_memory = STANDARD_HLT_PRINTS_MEMORY;
    params->max_evaluated = STANDARD_MAX_EVALUATE;
    params->max_time = STANDARD_MAX_TIME;
    params->debug_mode = STANDARD_DEBUG;

    return params;
}

// Interpreta o arquivo
ErrorCode_t interpret(FILE * file, Parametros * params) {

    // Inicializa os parâmetros
    params->real_max_time = params->max_time;

    // Obtem os tokens do arquivo
    Token_t * tokens = NULL;
    size_t tokens_size = 0;

    ErrorCode_t err = tokenize(file, &tokens, &tokens_size);
    if (err != EXIT_SUCCESS || tokens == NULL || tokens[0].type == TokenType_EOF) {
        // Retorna o erro
        return err;
    }

    // Inicializa o ambiente do SAP2
    Environment env = {
        .memory = calloc(MEMORY_SIZE, sizeof(memoryUnit_t)),
        .programCounter = params->start_address,
        .registers = calloc(NUMBER_OF_REGISTERS, sizeof(hex1_t)),
        .symbolTable = NULL,
        .symbolCount = 0,

        .params = params,
        .currentInstruction = 0,
        .totalInstructions = 0,
        .usedAddresses = NULL,
        .usedAddressesSize = 0,
        .hex_print_buffer = 0,
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
    env.programCounter = params->start_address;
    env.currentInstruction = 1;

    // Avalia(executa) o código
    ErrorCode_t exit_code = evaluate(&env);

    // Fim do código //

    // Antes de sair, imprime a memória
    // Obs.: A verificação de "env->usedAddressesSize > 0" é praticamente desnecessária,
    // já que, para chegar aqui, precisaria de ter o OPCODE do HLT na memória
    // (portanto, algum endereço da memória foi usado).
    if (env.usedAddressesSize > 0 && env.params->hlt_prints_memory) {
        print_info(&env);
    }

    // Verifica se a última instrução foi um HLT. Se
    // não for, avisa ao usuário.
    if (env.last_instruction.value != OPCODE_HLT) {
        WARN(
            "A ultima instrucao do codigo foi \"%s\" (Instrucao %d)\nao inves de um HLT! Certifique-se de colocar uma instrucao HLT\nno fim de seu codigo.",
            env.last_instruction.annotation,
            env.last_instruction.nInstruction);
    }

    // Libera os tokens
    for (size_t i = 0; i < tokens_size; i++) {
        free(tokens[i].value);
    }
    free(tokens);

    // Libera o ambiente depois das execuções
    free(env.memory);
    free(env.registers);

    // Retorna sucesso
    return exit_code;
}