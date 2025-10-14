//
//
// Author: André
// Date: 18/09/2025
//

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <stdarg.h> // VA ARGS

// Importa o respectivo header para usar sleep
#ifdef _WIN32 // Verifica se é Windows
    #include <pthread.h>
    #include <time.h>
#else
    #ifdef __linux__ // Verifica se é linux
        #include <unistd.h> // usleep
        //#include <linux/time.h>
    #endif // __linux__
#endif // _WIN32


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
    if (endp == buf || *endp != '\0') return EXIT_INVALID_ARGUMENT;
    if (tmp > MAX_INT_TO_HEX) return EXIT_ILLEGAL_HEX;
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
    if (endp == buf || *endp != '\0') return EXIT_INVALID_ARGUMENT;
    if (val > UHEX2_MAX) return EXIT_ILLEGAL_HEX;

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

void enter_to_continue() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void windows_sleep_us(long microseconds) {
    #ifdef _WIN32 // se for windows
    // Se a espera for muito curta, o overhead do timer não compensa,
    // então faz busy-wait.
    if (microseconds < 1000) {
        LARGE_INTEGER frequency, start, end;
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
        double limit = (double)microseconds / 1000000.0;
        do {
            QueryPerformanceCounter(&end);
        } while ((double)(end.QuadPart - start.QuadPart) / frequency.QuadPart < limit);
        return;
    }

    // Para esperas mais longas, usamos o Waitable Timer para ser eficiente
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10LL * microseconds);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    if (timer == NULL)
        return; // Falha ao criar o timer


    // Define o timer para disparar após o tempo especificado
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);

    // Espera até que o timer dispare
    WaitForSingleObject(timer, INFINITE);

    // Limpa o recurso
    CloseHandle(timer);
    #endif
}

void sleep_us(long microseconds) {
    #ifdef _WIN32
        windows_sleep_us(microseconds);
    #else
        // Se for linux, usa usleep
        #ifdef __linux__
            // Busy-wait para espera curtas
            if (microseconds < 1000) {
                struct timespec start, current;
                clock_gettime(CLOCK_MONOTONIC, &start);
                long long start_ns = (long long)start.tv_sec * 1000000000 + start.tv_nsec;
                long long limit_ns = microseconds * 1000;

                while (1) {
                    clock_gettime(CLOCK_MONOTONIC, &current);
                    long long current_ns = (long long)current.tv_sec * 1000000000 + current.tv_nsec;

                    if ((current_ns - start_ns) >= limit_ns) break;
                    
                }
                
            // Para esperas mais longas, usleep é eficiente.
            } else {
                usleep(microseconds);
            }
        #endif //__linux__
    #endif // _WIN32
}

void stopWatch_start(stopWatch_s* sw) {
    sw->elapsed_time = 0;
    #ifdef _WIN32
        QueryPerformanceFrequency(&sw->frequency);
        QueryPerformanceCounter(&sw->start_time);
    #else
    #ifdef __linux__
        clock_gettime(CLOCK_MONOTONIC, &sw->start_time);
    #endif
    #endif
}

void stopWatch_end(stopWatch_s* sw) {
#ifdef _WIN32
    LARGE_INTEGER end_time;
    QueryPerformanceCounter(&end_time);

    sw->elapsed_time = (double)(end_time.QuadPart - sw->start_time.QuadPart) / sw->frequency.QuadPart;
#else
#ifdef __linux__
    struct timespec end_time;
    clock_gettime(CLOCK_MONOTONIC, &end_time);

    sw->elapsed_time = (end_time.tv_sec - sw->start_time.tv_sec)
                     + (end_time.tv_nsec - sw->start_time.tv_nsec) / 1000000000.0;
#endif
#endif
}

double stopWatch_timeElapsed(stopWatch_s* sw) {
    if (sw->elapsed_time != 0)
        return sw->elapsed_time;

    #ifdef _WIN32
        LARGE_INTEGER end_time;
        QueryPerformanceCounter(&end_time);

        return (double)(end_time.QuadPart - sw->start_time.QuadPart) / sw->frequency.QuadPart;
    #else
    #ifdef __linux__
        struct timespec end_time;
        clock_gettime(CLOCK_MONOTONIC, &end_time);

        return (end_time.tv_sec - sw->start_time.tv_sec)
               + (end_time.tv_nsec - sw->start_time.tv_nsec) / 1000000000.0;
    #endif
    #endif
}

void print_binary(int size, int num) {
    size *= 8;
    // Imprime cada bit começando do MSB
    for (int i = size-1; i >= 0; i--) {
        // Arrasta o número i vezes e usa AND com 1
        // para determinar se o valor é 0 ou 1
        printf("%d", (num >> i) & 1);
        // De 4 em 4 bits, dá um es
        if (i % 4 == 0 && i != 0) printf(" ");
    }
}
