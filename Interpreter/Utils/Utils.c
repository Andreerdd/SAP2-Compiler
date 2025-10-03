//
//
// Author: André
// Date: 18/09/2025
//

#include <stdint.h>
#include <stdarg.h> // VA ARGS

#include "Utils.h"
#include "../ErrorCodes.h"
#include "../environment.h"

#define HEX_BUFFER 32

/**
 * Adiciona o caractere dado ao fim da string. Se não conseguir
 * adicionar, não altera a string original.
 * @param c o caractere dado
 * @param array a string (ponteiro que pode ser realocado)
 * @param size o tamanho atual da string (contando com o '\0')
 * @return o novo tamanho da string (-1 se der algum erro)
 */
size_t addCharToString(char c, char** array, size_t size) {
    // Realoca com espaço para o novo caractere + '\0'
    char* tmp = realloc(*array, (size + 2) * sizeof(char));
    if (tmp == NULL)
        return -1;
    *array = tmp;

    // Coloca o caractere no fim do vetor e o '\0'
    (*array)[size] = c;
    (*array)[size + 1] = '\0';
    return size + 1;
}

errno_t str_to_hex1(const char* text, hex1_t* out) {
    if (!text)
        return EXIT_NULL_ARGUMENT;

    size_t len = strlen(text);
    if (len < 2 || text[len-1] != 'H') return EXIT_INVALID_ARGUMENT; // formato inválido

    char buf[HEX_BUFFER];
    if (len >= sizeof(buf)) return EXIT_INVALID_ARGUMENT; // muito longo
    memcpy(buf, text, len-1);
    buf[len-1] = '\0';

    char* endp = NULL;
    unsigned long tmp = strtoul(buf, &endp, 16);
    if (endp == buf || *endp != '\0' || tmp > MAX_INT_TO_HEX) return EXIT_INVALID_ARGUMENT;
    *out = (hex1_t)(uint8_t)tmp;

    return EXIT_SUCCESS;
}

errno_t str_to_hex2(const char* text, hex2_t* out) {
    if (!text)
        return EXIT_NULL_ARGUMENT;

    size_t len = strlen(text);
    if (len < 2 || text[len-1] != 'H') return EXIT_INVALID_ARGUMENT; // formato inválido

    char buf[HEX_BUFFER];
    if (len >= sizeof(buf)) return EXIT_INVALID_ARGUMENT; // muito longo
    memcpy(buf, text, len-1);
    buf[len-1] = '\0';

    char* endp = NULL;
    unsigned long val = strtoul(buf, &endp, 16);
    if (endp == buf || *endp != '\0' || val > HEX2_MAX) return EXIT_INVALID_ARGUMENT;

    *out = (hex2_t)val;
    return EXIT_SUCCESS;
}

int comp_hex2(const void * a, const void * b) {
    hex2_t xa = *(hex2_t*)a;
    hex2_t xb = *(hex2_t*)b;
    return (xa > xb) - (xa < xb);
}

char* formatString(const char* format, ...) {
    char* string = NULL;
    size_t size;

    va_list vargs1;
    va_start(vargs1, format);

    // Cria uma cópia da lista de argumentos
    va_list vargs2;
    va_copy(vargs2, vargs1);

    // Mede o tamanho necessario
    size = vsnprintf(NULL, 0, format, vargs1);
    va_end(vargs1);

    if (size < 0) {
        va_end(vargs2);
        return NULL; // Erro de codificação no formato
    }

    // Aloca memória
    string = malloc(sizeof(char) * (size + 1));
    if (string == NULL) {
        va_end(vargs2);
        return NULL;
    }
    // Formata a string
    vsnprintf(string, size + 1, format, vargs2);
    va_end(vargs2);

    // Retorna a string alocada
    return string;
}