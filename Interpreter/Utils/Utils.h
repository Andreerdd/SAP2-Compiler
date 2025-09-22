//
//
// Author: André
// Date: 17/09/2025
//

#ifndef SAP2_COMPILER_UTIL_H
#define SAP2_COMPILER_UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// Para concatenar dois valores
#define CAT2(a,b) a##b

/**
 * Adiciona o caractere dado ao fim da string. Se não conseguir
 * adicionar, não altera a string original.
 * @param c o caractere dado
 * @param array a string (ponteiro que pode ser realocado)
 * @param size o tamanho atual da string (contando com o '\0')
 * @return o novo tamanho da string (-1 se der algum erro)
 */
size_t addCharToString(char c, char** array, size_t size);

/**
 * Converte um texto em um número hexadecimal
 * @param text hexadecimal em texto (com H no fim)
 * @param out endereço de onde vai se guardar o resultado (hex)
 * @return código de erro
 */
errno_t str_to_hex1(const char* text, uint8_t* out);

/**
 * Converte um texto em um número hexadecimal com 8 bits
 * @param text hexadecimal em texto (com H no fim)
 * @param out endereço de onde vai se guardar o resultado (hex2)
 * @return código de erro
 */
errno_t str_to_hex2(const char* text, uint16_t* out);

/**
 * Compara dois inteiros
 * @param a valor inteiro
 * @param b valor inteiro
 * @return se (a > b), retorna 1. Se (a < b), retorna -1. Se (a == b), retorna 0.
 */
int comp_hex2(const void * a, const void * b);
#endif //SAP2_COMPILER_UTIL_H