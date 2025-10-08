#include <stdio.h>
#include <string.h>

#include "Interpreter/ErrorCodes.h"
#include "Interpreter/interpreter.h"
#include "Interpreter/Utils/Utils.h"

// Compara o argumento atual com a string dada
#define cmp_curr_str(s) \
    (strcmp(argv[i], s) == 0)
// Compara o argumento atual com a string dada e a forma reduzida dela
#define cmp_curr_str_r(s, r) \
    (strcmp(argv[i], s) == 0 || strcmp(argv[i], r) == 0)
// Incrementa e verifica o i
#define inr \
    i++; \
    if (i >= argc) { \
        V_EXIT(EXIT_NULL_ARGUMENT, "Um valor eh esperado depois do parametro %s.", argv[i-1]);   \
    } \

Parametros * getParametros(int argc, char ** argv) {
    Parametros * parametros = get_standard_parameters();

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--inicio") == 0 || strcmp(argv[i], "-i") == 0) {
            inr;
            hex2_t inicio;
            if (str_to_hex2(argv[i], &inicio) == EXIT_SUCCESS ) {
                parametros->start_address = (uhex2_t)inicio;
            } else {
                V_EXIT(
                EXIT_INVALID_ARGUMENT,
"O parametro \"%s\" espera um hexadecimal mas foi encontrado o valor \"%s\".\n       Verifique se:\n\t- Esse valor esta no formato de um hexadecimal \"1234H\";\n\t- Se esse valor nao foi escrito no lugar errado;\n\t- Se ha apenas um espaco entre o parametro e o valor.",
                argv[i-1],
                argv[i]
                );
            }
        }
        else if (cmp_curr_str_r("--limite", "-l")) {
            inr;
            char* endptr = NULL;
            int v = (int) strtol(argv[i], &endptr, 10);
            if (strlen(endptr) > 0) {
                V_EXIT(EXIT_INVALID_ARGUMENT,
                "O parametro \"%s\" espera um inteiro positivo depois mas foi encontrado o valor \"%s\".\nVerifique se esse valor eh um numero inteiro positivo.",
                argv[i-1],
                argv[i]
                );
            }
            parametros->max_evaluated = v;
        }
        else if (cmp_curr_str_r("--saida-limpa", "-sl")) {
            parametros->hlt_prints_memory = false;
        }
        else if (cmp_curr_str_r("--debug", "-d") || cmp_curr_str_r("--passo-a-passo", "-p")) {
            parametros->debug_mode = true;
        }
        else {
            // Verifica se é algum tipo de valor para um parâmetro //
            // Verifica se é um número
            char* endptr = NULL;
            (void)strtol(argv[i], &endptr, 10);
            if (strlen(endptr) == 0) {
                continue;
            }

            // Verifica se é um hexadecimal
            hex2_t vx;
            if (str_to_hex2(argv[i], &vx) == EXIT_SUCCESS) {
                continue;
            }

            WARN("Parametro desconhecido: %s", argv[i]);
        }
    }

    return parametros;
}

int main(int argc, char ** argv) {
    // Se tiver apenas 1 argumento(só a localização do executável),
    // mostra como funciona
    if (argc <= 1)
        RETURN_ERR(EXIT_NO_FILE);

    // Obtém o arquivo
    FILE * file = fopen(argv[1], "r");
    if (file == NULL)
        RETURN_ERR(EXIT_FILE_NOT_FOUND);

    // Obtém os parâmetros
    Parametros * parametros = getParametros(argc, argv);

    // Interpreta o arquivo usando os parâmetros dados
    ErrorCode_t err = interpret(file, parametros);

    // Finaliza o programa
    free(parametros);
    return err;
}
