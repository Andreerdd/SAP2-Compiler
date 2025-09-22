//
//
// Author: André
// Date: 18/09/2025
//

#include <stdint.h>

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

errno_t str_to_hex1(const char* text, uhex1_t* out) {
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
    if (endp == buf || *endp != '\0' || val > UHEX1_MAX) return EXIT_INVALID_ARGUMENT;

    *out = (uhex1_t)val;
    return EXIT_SUCCESS;
}

errno_t str_to_hex2(const char* text, uhex2_t* out) {
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
    if (endp == buf || *endp != '\0' || val > UHEX2_MAX) return EXIT_INVALID_ARGUMENT;

    *out = (uhex2_t)val;
    return EXIT_SUCCESS;
}

int comp_hex2(const void * a, const void * b) {
    uhex2_t xa = *(uhex2_t*)a;
    uhex2_t xb = *(uhex2_t*)b;
    return (xa > xb) - (xa < xb);
}