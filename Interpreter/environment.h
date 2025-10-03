// O ambiente do SAP2. Usado para simular a memória, por exemplo.
//
// Author: André
// Date: 19/09/2025
//

#ifndef SAP2_COMPILER_ENVIRONMENT_H
#define SAP2_COMPILER_ENVIRONMENT_H

#define NUMBER_OF_REGISTERS 3
#define REGISTER_A 0
#define REGISTER_B 1
#define REGISTER_C 2
#define ACCUMULATOR (REGISTER_A)

#define NUMBER_OF_FLAGS 2
#define FLAG_S 0
#define FLAG_Z 1

// Endereços que guardarão o endereço onde RET direcionará
#define RET_ADDRESS_LSB 0x0001
#define RET_ADDRESS_MSB 0x0002

#define MEMORY_SIZE UINT16_MAX // 65535 (que é 2 hexadecimais, 0xFFFF)
#define STARTER_MEMORY_ADDRESS 0x8000 // entre 0000H e 07FFH, está o programa monitor

#define HEX1_MAX INT8_MAX
#define HEX1_MIN INT8_MIN
#define HEX2_MAX INT16_MAX
#define UHEX2_MAX UINT16_MAX

#define MAX_INT_TO_HEX 0xFF

#include <stdbool.h>
#include <stdint.h>

// Valor de um hexadecimal (0xFF)
typedef int8_t hex1_t;
// Valor de um hexadecimal (0xFF)
typedef uint8_t uhex1_t;
// Valor de dois hexadecimais (0xFFFF)
typedef int16_t hex2_t;
// Valor de dois hexadecimais sem sinal (0xFFFF)
typedef uint16_t uhex2_t;

// Rótulo
typedef struct {
    char* name;
    uhex2_t address;
} label_t;

// "Unidade de memória", criada para poder "anotar"
// na memória
#define EMPTY_ANNOTATION ""
typedef struct {
    hex1_t value;
    char* annotation;
} memoryUnit_t;

// Ambiente do SAP2
typedef struct {
    // A memória RAM
    memoryUnit_t * memory;
    // Último endereço usado no escopo principal do programa
    uhex2_t programCounter;
    // Registradores
    hex1_t * registers;
    // Tabela de Símbolos
    label_t * symbolTable;
    size_t symbolCount;
    // Flags
    int flags[NUMBER_OF_FLAGS];
    // Se é a primeira passagem (carrega os rótulos)
    bool isFirstPass;

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
void addToMemoryHex1(Environment * env, hex1_t hex);

/**
 * Adiciona à memória dois hexadecimais. Esse conteúdo será
 * adicionado na posição que o "Contador de Programa" está.
 * @param env o ambiente do SAP2
 * @param hex o conteúdo
 */
void addToMemoryHex2(Environment * env, hex2_t hex);

/**
 * Retorna se o endereço dado já foi usado com busca binária
 * @param env o ambiente do SAP2
 * @param address o endereço que se quer saber se foi usado
 * @return se o endereço foi usado ou não
 */
int isAddressUsed(Environment * env, uhex2_t address);

/**
 * Liga o nome dado ao endereço de memória dado, criando
 * um rótulo(label).
 * @param env o ambiente do SAP2
 * @param name o nome do rótulo
 * @param address o endereço de memória ligado ao rótulo
 */
void addLabel(Environment * env, char * name, uhex2_t address);

/**
 * Obtém o endereço de memória ligado ao nome dado
 * @param env o ambiente do SAP2
 * @param name o nome do rótulo
 * @return endereço de memória do rótulo
 */
uhex2_t getAddressOfLabel(Environment * env, char * name);

/**
 *
 * @param env o ambiente do SAP2
 * @param text texto que deve ser colocado
 */
void appendAnnotationToLastMemoryUnit(Environment * env, char * text);

/**
 * Adiciona a instrução na memória
 * @param env o ambiente do SAP2
 * @param opcode Código de operação da instrução
 */
void addInstruction(Environment * env, uhex1_t opcode);

/**
 * Adiciona a instrução na memória e o valor hexadecimal logo em seguida
 * @param env o ambiente do SAP2
 * @param opcode Código de operação da instrução
 * @param value o valor hexadecimal (8 bytes, 1 hexadecimal)
 */
void addInstructionWithHex1(Environment * env, uhex1_t opcode, hex1_t value);

/**
 * Adiciona a instrução na memória, o LSB(byte menos importante) e o
 * MSB(byte mais importante) logo em seguida.
 * @param env o ambiente do SAP2
 * @param opcode Código de operação da instrução
 * @param value o valor do hexadecimal (16 bytes, 2 hexadecimais)
 */
void addInstructionWithHex2(Environment * env, uhex1_t opcode, hex2_t value);

/**
 * Define o registrador dado com o valor dado.
 * @param env o ambiente do SAP2
 * @param reg o registrador que se quer definir o valor
 * @param value o novo valor do registrador
 */
void setRegister(Environment * env, int reg, hex1_t value);
#endif //SAP2_COMPILER_ENVIRONMENT_H