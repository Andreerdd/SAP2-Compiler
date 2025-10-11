// O ambiente do SAP2. Usado para simular a memória, por exemplo.
//
// Author: André
// Date: 19/09/2025
//
#pragma once

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

#define MEMORY_SIZE UINT16_MAX // 65535 (que é 2 hexadecimais, 0xFFFF)
#define STANDARD_STARTER_MEMORY_ADDRESS 0x8000 // entre 0000H e 07FFH, está o programa monitor

// Endereços que guardarão o endereço onde RET direcionará
#define RET_ADDRESS_LSB (MEMORY_SIZE - 2)
#define RET_ADDRESS_MSB (MEMORY_SIZE - 1)

// Valores máximos dos hexadecimais
#define HEX1_MAX INT8_MAX
#define HEX1_MIN INT8_MIN
#define HEX2_MAX INT16_MAX
#define UHEX2_MAX UINT16_MAX

// Maior valor HEX
#define MAX_INT_TO_HEX 0xFF

// Maior quantidade de algarismos de um número com 1 hexadecimal
#define MAX_LENGTH_SINGLE_HEX (2 + 1) // FFH

// Macros que retornam a entrada/saída de cada byte
#define _in_flow(x) stdin
#define _out_flow(x) stdout

#define stdoutflow _out_flow(0)
#define stdinflow _in_flow(0)

// Quantos T-States são precisos para alterar o PC
#define _ts_to_change_pc 3


#include <stdbool.h>
#include <stdint.h>

#define env_params env->params
#define print_hex(f, xv) fprintf(f, "%xH (Decimal: %d)\n", (uhex1_t)xv, xv)


// Valor de um hexadecimal (0xFF)
typedef int8_t hex1_t;
// Valor de um hexadecimal (0xFF)
typedef uint8_t uhex1_t;
// Valor de dois hexadecimais (0xFFFF)
typedef int16_t hex2_t;
// Valor de dois hexadecimais sem sinal (0xFFFF)
typedef uint16_t uhex2_t;


#define STANDARD_HLT_PRINTS_MEMORY true
#define STANDARD_MAX_EVALUATE (-1)
#define STANDARD_MAX_TIME (10000)
#define STANDARD_DEBUG false
// Os parâmetros para a interpretação do arquivo dado
typedef struct {
    // Endereço que o contador de programa iniciará
    uhex2_t start_address;
    // Se o HLT vai imprimir a memória
    bool hlt_prints_memory;
    // Quantidade máxima de instruções que podem ser executadas
    int max_evaluated;
    // Quantidade máxima de tempo que o programa pode durar (em milissegundos)
    double max_time;
    // Se o modo de depuração está ativo
    bool debug_mode;
} Parametros;

// Rótulo
typedef struct {
    char* name;
    uhex2_t value;
} label_t;


// "Unidade de memória", criada para poder "anotar"
// na memória
typedef struct {
    hex1_t value;
    char* annotation;
    union {
        long nInstruction;
    } temp;
} memoryUnit_t;
#define EMPTY_ANNOTATION "" // Anotação vazia
#define EVAL_DEFINED_MEMORY_ANNOTATION "Valor definido por uma instrucao" // Quando o trecho é definido por um setMemory()


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
    // Os parâmetros passados para o usuário
    Parametros * params;
    // Instrução atual (quantas instruções já se passaram)
    unsigned int currentInstruction;
    // Endereços usados
    uhex2_t * usedAddresses;
    // Quantidade de endereços usados
    size_t usedAddressesSize;
    // A última instrução avaliada
    memoryUnit_t last_instruction;
    // Um valor que está esperando para ser impresso. Útil
    // no processo de depuração, em que só deve ser impresso
    // depois que as informações forem impressas
    hex1_t hex_print_buffer;
    // O valor do fluxo de dados que está esperando para ser usado.
    hex2_t hex_flow_buffer;
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
uhex2_t getValueOfLabel(Environment * env, char * name);

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
 * Define o registrador dado com o valor dado e, se for o
 * acumulador, atualiza os flags.
 * @param env o ambiente do SAP2
 * @param reg o registrador que se quer definir o valor
 * @param value o novo valor do registrador
 */
void setRegister(Environment * env, int reg, hex1_t value);

/**
 * Guarda o valor dado no endereço de memória dado.
 * @param env o ambiente do SAP2
 * @param address o endereço de memória que se quer guardar o valor
 * @param value o valor que se quer guardar
 */
void setMemory(Environment * env, uhex2_t address, hex1_t value);

/**
 * Guarda o valor dado no endereço da memória dado (para o LSB) e no
 * seguinte (para o MSB)
 * @param env o ambiente do SAP2
 * @param address o endereço de memória que se quer guardar o valor
 * @param value o valor que se quer guardar
 */
void setMemoryHex2(Environment * env, uhex2_t address, hex2_t value);

/**
 * Guarda o valor dado no endereço de memória dado e com a anotação dada.
 * @param env o ambiente do SAP2
 * @param address o endereço de memória que se quer guardar o valor
 * @param value o valor que se quer guardar
 * @param annotation a anotação que se quer fazer naquele endereço.
 */
void setMemoryWithAnnotation(Environment * env, uhex2_t address, hex1_t value, const char * annotation);

/**
 * Imprime a memória do SAP2
 * @param env o ambiente do SAP2
 */
void print_memory(Environment * env);

/**
 * Imprime os flags do SAP2
 * @param env o ambiente do SAP2
 */
void print_flags(Environment * env);

/**
 * Imprime as informações do SAP2
 * @param env o ambiente do SAP2
 */
void print_info(Environment * env);

/**
 * Imprime as informações de depuração da interpretação do código
 * @param env o ambiente do SAP2
 */
void print_debug_info(Environment * env);

/**
 * Lê um hexadecimal do fluxo de dados de entrada dado
 * @param flow o número que representa o lugar do fluxo de dados
 * @return Hexadecimal lido
 */
hex1_t get_1hex_from_in(uhex1_t flow);
#endif //SAP2_COMPILER_ENVIRONMENT_H