//
//
// Author: André
// Date: 19/09/2025
//

#include <stdint.h> // uint8_t

#include "environment.h"
#include "ErrorCodes.h"
#include "Utils/Utils.h"

/**
 * Função para encontrar o ponto de inserção de um endereço
 * usando busca binária.
 * @param arr array de uhex2_t
 * @param size tamanho do array
 * @param key elemento que se quer encontrar
 * @return posição para colocar
 */
int find_address_insertion_point(uhex2_t arr[], int size, uhex2_t key) {
    int low = 0;
    int high = size - 1;
    int mid;

    // Busca binária
    while (low <= high) {
        mid = low + (high - low) / 2;
        if (arr[mid] == key) {
            // Encontrou uma duplicata
            return mid;
        }
        if (arr[mid] < key) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    // Se o elemento não foi encontrado, low é a posição certa
    return low;
}

/**
 * Insere o endereço de uhex2_t dado em um array.
 * @param arr array de uhex2_t
 * @param size tamanho do array
 * @param element elemento que se quer adicionar
 */
void insertAddressIntoMemory(uhex2_t arr[], size_t * size, uhex2_t element) {
    // Acha a posição para o novo elemento
    int pos = find_address_insertion_point(arr, *size, element);

    // Desloca os elementos para abrir espaço para o novo elemento
    for (size_t i = *size; i > pos; i--) {
        arr[i] = arr[i - 1];
    }

    // Insere o novo elemento
    arr[pos] = element;
    (*size)++; // Incrementa o tamanho do array
}

int charToRegister(char c) {
    switch (c) {
        case 'A': return ACCUMULATOR;
        case 'B': return REGISTER_B;
        case 'C': return REGISTER_C;
        default: return -1;
    }
}

/**
 * Adiciona um endereço ao endereços usados
 * @param env o ambiente do SAP2
 * @param address endereço que se quer adicionar aos endereços usados
 */
void addAddressToUsedMemory(Environment * env, uhex2_t address) {
    // Realoca para adicionar o endereço
    size_t newSize = env->usedAddressesSize + 1;
    uhex2_t * temp = (uhex2_t *)realloc(env->usedAddresses, sizeof(uhex2_t) * newSize);
    if (temp == NULL) {
        WARN("Instrucao %d: os dados dessa instrucao podem nao ter sido corretamente guardados na tabela de memoria usada.",
            env->currentInstruction);
        return;
    }
    env->usedAddresses = temp;

    insertAddressIntoMemory(env->usedAddresses, &env->usedAddressesSize, address);
}

int isAddressUsed(Environment * env, uhex2_t address) {
    int *found_item = bsearch(&address, env->usedAddresses, env->usedAddressesSize, sizeof(uhex2_t), comp_hex2);
    return found_item != NULL;
}

void addToMemoryHex1(Environment * env, uint8_t hex) {
    if (env->memory == NULL)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "Erro interno: a memoria nao foi alocada.\nVerifique se ha memoria disponivel no dispositivo.");
    if (env->programCounter >= MEMORY_SIZE)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "A memoria RAM esta cheia.\nPossivelmente, seu codigo ultrapassou o limite de memoria.");
    if (env->programCounter < STARTER_MEMORY_ADDRESS)
        V_EXIT(EXIT_READ_ONLY_ADDRESS,
            "A posicao de memoria \"%x\" eh de apenas leitura (Read-Only)\nmas tentaram escrever nela.", env->programCounter);

    // Se chegou aqui, a memória ainda está no intervalo

    // Verifica se tem algo nela (para avisar o usuário que ele
    // está sobrescrevendo algo)
    if (isAddressUsed(env, env->programCounter))
        WARN("A posicao de memoria \"%x\" vai ser sobrescrita, mas ha conteudo nela.\nIsso pode causar comportamentos inesperados.", env->programCounter);

    // Sobrescreve e incrementa o contador de programa
    env->memory[env->programCounter] = hex;
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;
}

void addToMemoryHex2(Environment * env, uhex2_t hex) {
    if (env->memory == NULL)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "Erro interno: a memoria nao foi alocada.\nVerifique se ha memoria disponivel no dispositivo.");
    if (env->programCounter+1 >= MEMORY_SIZE) // +1 porque gasta 2 endereços
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "A memoria RAM esta cheia.\nPossivelmente, seu codigo ultrapassou o limite de memoria.");
    if (env->programCounter < STARTER_MEMORY_ADDRESS)
        V_EXIT(EXIT_READ_ONLY_ADDRESS,
            "A posicao de memoria \"%x\" eh de apenas leitura (Read-Only)\nmas tentaram escrever nela.", env->programCounter);

    // Se chegou aqui, a memória ainda está no intervalo

    // Verifica se tem algo nela (para avisar o usuário que ele
    // está sobrescrevendo algo)
    if (isAddressUsed(env, env->programCounter) || isAddressUsed(env, env->programCounter+1))
        WARN("A posicao de memoria \"%x\" vai ser sobrescrita, mas ha conteudo nela.\nIsso pode causar comportamentos inesperados.", env->programCounter);

    // Escreve o LSB
    env->memory[env->programCounter] = hex & 0xFF;
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;

    // Escreve o MSB
    env->memory[env->programCounter] = hex >> 8;
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;
}
