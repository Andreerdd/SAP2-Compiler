//
//
// Author: André
// Date: 17/09/2025
//

#ifndef SAP2_COMPILER_UTIL_H
#define SAP2_COMPILER_UTIL_H

// Headers necessários para o stopwatch
#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "../environment.h"

// Para concatenar dois valores
#define CAT2(a,b) a##b

// Para transformar um valor uhex1 em string
#define toStringUHex(xv) formatString("%xH", xv)

// Para transformar segundos em milissegundos
#define seg_to_ms(s) (s * 1000)

// Para transformar milissegundos em segundos
#define ms_to_seg(s) (s / 1000)

// Estrutura que representa um cronômetro
typedef struct {
    // Tempo passado (em segundos)
    double elapsed_time;
    #ifdef _WIN32
        // Cronômetro para Windows
        LARGE_INTEGER frequency;
        LARGE_INTEGER start_time;
    #else
    #ifdef __linux__
        // Cronômetro para Linux
        struct timespec start_time;
    #endif
    #endif
} stopWatch_s;

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
errno_t str_to_hex1(const char* text, hex1_t* out);

/**
 * Converte um texto em um número hexadecimal com 8 bits
 * @param text hexadecimal em texto (com H no fim)
 * @param out endereço de onde vai se guardar o resultado (hex2)
 * @return código de erro
 */
errno_t str_to_hex2(const char* text, hex2_t* out);

/**
 * Compara dois inteiros
 * @param a valor inteiro
 * @param b valor inteiro
 * @return se (a > b), retorna 1. Se (a < b), retorna -1. Se (a == b), retorna 0.
 */
int comp_hex2(const void * a, const void * b);

/**
 * Formata um texto no estilo "printf"
 * @param format string base
 * @param ... outras strings
 * @return string formatada
 */
char* formatString(const char* format, ...);

/**
 * Para o programa até o usuário dar enter (quebra de linha),
 * limpando o que o usuário digitou
 */
void enter_to_continue();

/**
 * Congela o código pelo tempo dado.
 * @param microseconds tempo, em microssegundos, para esperar
 */
void sleep_us(long microseconds);

/**
 * Inicia o cronômetro dado.
 * @param sw o cronômetro
 */
void stopWatch_start(stopWatch_s* sw);

/**
 * Para o cronômetro dado.
 * @param sw o cronômetro
 */
void stopWatch_end(stopWatch_s* sw);

/**
 * Retorna o tempo (em segundos) que o cronômetro durou. Se o
 * cronômetro não tiver sido parado ainda, retorna o tempo (em
 * segundos) passado desde o início do cronômetro dado.
 * @param sw o cronômetro
 * @return tempo que o cronômetro durou (ou está durando)
 */
double stopWatch_timeElapsed(stopWatch_s* sw);

/**
 * Imprime os bits do número dado
 * @param size tamanho do número em bytes
 * @param num número que se quer imprimir os bits
 */
void print_binary(int size, int num);
#endif //SAP2_COMPILER_UTIL_H