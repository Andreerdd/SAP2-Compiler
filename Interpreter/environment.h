// O ambiente do SAP2. Usado para simular a memória, por exemplo.
//
// Author: André
// Date: 19/09/2025
//

#ifndef SAP2_COMPILER_ENVIRONMENT_H
#define SAP2_COMPILER_ENVIRONMENT_H

#define NUMBER_OF_REGISTERS 3
#define ACCUMULATOR 0
#define REGISTER_B 1
#define REGISTER_C 2

#define MEMORY_SIZE UINT16_MAX // 65535 (que é 2 hexadecimais, 0xFFFF)
#define STARTER_MEMORY_ADDRESS 0x8000 // entre 0000H e 07FFH, está o programa monitor

#define UHEX1_MAX UINT8_MAX // (0xFF)
#define UHEX2_MAX UINT16_MAX // (0xFFFF)

#include <stdint.h>

// Valor de um hexadecimal (0xFF)
typedef uint8_t uhex1_t;
// Valor de dois hexadecimais (0xFFFF)
typedef uint16_t uhex2_t;

typedef struct {
    // A memória RAM
    uhex1_t * memory;
    // Último endereço usado no escopo principal do programa
    uhex2_t programCounter;
    // Registradores
    uhex1_t * registers;

    // Extras //
    // Instrução atual (quantas instruções já se passaram)
    unsigned int currentInstruction;
    // Endereços usados
    uhex2_t * usedAddresses;
    // Quantidade de endereços usados
    size_t usedAddressesSize;
} Environment;

/**
 * Retorna o inteiro que representa o respectivo registrador
 * @param c caractere com a letra que representa um registrador
 * @return inteiro do registrador dado. (-1) se não encontrar
 */
int charToRegister(char c);

/**
 * Adiciona à memória um hexadecimal. Esse conteúdo será
 * adicionado na posição que o "Contador de Programa" está.
 * @param env o ambiente do SAP2
 * @param hex o conteúdo
 */
void addToMemoryHex1(Environment * env, uhex1_t hex);

/**
 * Adiciona à memória dois hexadecimais. Esse conteúdo será
 * adicionado na posição que o "Contador de Programa" está.
 * @param env o ambiente do SAP2
 * @param hex o conteúdo
 */
void addToMemoryHex2(Environment * env, uhex2_t hex);

/**
 * Retorna se o endereço dado já foi usado com busca binária
 * @param env o ambiente do SAP2
 * @param address o endereço que se quer saber se foi usado
 * @return se o endereço foi usado ou não
 */
int isAddressUsed(Environment * env, uhex2_t address);
#endif //SAP2_COMPILER_ENVIRONMENT_H