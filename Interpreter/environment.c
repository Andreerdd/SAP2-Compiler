//
//
// Author: André
// Date: 19/09/2025
//

#include <stdint.h> // uint8_t

#include "environment.h"

#include <ctype.h>

#include "ErrorCodes.h"
#include "Instructions/Instructions.h"
#include "Utils/Utils.h"


/**
 * Função para encontrar o ponto de inserção de um endereço
 * usando busca binária.
 * @param arr array de uhex2_t
 * @param size tamanho do array
 * @param key elemento que se quer encontrar
 * @return posição para colocar
 */
int find_address_insertion_point(hex2_t arr[], int size, hex2_t key) {
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

int isAddressUsed(Environment * env, uhex2_t address) {
    int *found_item = bsearch(&address,
        env->usedAddresses,
        env->usedAddressesSize,
        sizeof(hex2_t),
        comp_hex2);
    return found_item != NULL;
}


/**
 * Adiciona um endereço ao endereços usados
 * @param env o ambiente do SAP2
 * @param address endereço que se quer adicionar aos endereços usados
 */
void addAddressToUsedMemory(Environment * env, uhex2_t address) {
    // Realoca para adicionar o endereço
    size_t newSize = env->usedAddressesSize + 1;
    uhex2_t * temp = realloc(env->usedAddresses, sizeof(uhex2_t) * newSize);
    if (temp == NULL) {
        WARN("Instrucao %d: os dados dessa instrucao podem nao ter sido corretamente guardados na tabela de memoria usada.",
            env->currentInstruction);
        return;
    }
    env->usedAddresses = temp;

    insertAddressIntoMemory(env->usedAddresses, &env->usedAddressesSize, address);
}

void addToMemoryHex1(Environment * env, hex1_t hex) {
    if (env->isFirstPass) {
        env->programCounter++;
        return;
    }
    if (env->memory == NULL)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "Erro interno: a memoria nao foi alocada.\nVerifique se ha memoria disponivel no dispositivo.");
    if (env->programCounter >= MEMORY_SIZE)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "A memoria RAM esta cheia.\nPossivelmente, seu codigo ultrapassou o limite de memoria.");
    if (env->programCounter < env_params->start_address)
        V_EXIT(EXIT_READ_ONLY_ADDRESS,
            "A posicao de memoria \"%x\" eh de apenas leitura (Read-Only)\nmas tentaram escrever nela.", env->programCounter);

    // Se chegou aqui, a memória ainda está no intervalo

    // Verifica se tem algo nela (para avisar o usuário que ele
    // está sobrescrevendo algo)
    if (isAddressUsed(env, env->programCounter))
        WARN("A posicao de memoria \"%x\" vai ser sobrescrita, mas ha conteudo nela.\nIsso pode causar comportamentos inesperados.", env->programCounter);

    // Sobrescreve e incrementa o contador de programa
    env->memory[env->programCounter] = (memoryUnit_t) {
        .value = hex,
        .annotation = EMPTY_ANNOTATION
    };
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;
}

void addToMemoryHex1Annotation(Environment * env, uhex1_t hex, char* text) {
    if (env->isFirstPass) {
        env->programCounter++;
        return;
    }
    if (env->memory == NULL)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "Erro interno: a memoria nao foi alocada.\nVerifique se ha memoria disponivel no dispositivo.");
    if (env->programCounter >= MEMORY_SIZE)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "A memoria RAM esta cheia.\nPossivelmente, seu codigo ultrapassou o limite de memoria.");
    if (env->programCounter < env_params->start_address)
        V_EXIT(EXIT_READ_ONLY_ADDRESS,
            "A posicao de memoria \"%x\" eh de apenas leitura (Read-Only)\nmas tentaram escrever nela.", env->programCounter);

    // Se chegou aqui, a memória ainda está no intervalo

    // Verifica se tem algo nela (para avisar o usuário que ele
    // está sobrescrevendo algo)
    if (isAddressUsed(env, env->programCounter))
        WARN("A posicao de memoria \"%x\" vai ser sobrescrita, mas ha conteudo nela.\nIsso pode causar comportamentos inesperados.", env->programCounter);

    // Sobrescreve e incrementa o contador de programa
    env->memory[env->programCounter] = (memoryUnit_t) {
        .value = (hex1_t)hex,
        .annotation = text
    };
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;
}

void addToMemoryHex2(Environment * env, hex2_t hex) {
    if (env->isFirstPass) {
        env->programCounter += 2;
        return;
    }
    if (env->memory == NULL)
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "Erro interno: a memoria nao foi alocada.\nVerifique se ha memoria disponivel no dispositivo.");
    if (env->programCounter+1 >= MEMORY_SIZE) // +1 porque gasta 2 endereços
        EXIT_CUSTOM_ERR(EXIT_NO_MEMORY,
            "A memoria RAM esta cheia.\nPossivelmente, seu codigo ultrapassou o limite de memoria.");
    if (env->programCounter < env_params->start_address)
        V_EXIT(EXIT_READ_ONLY_ADDRESS,
            "A posicao de memoria \"%x\" eh de apenas leitura (Read-Only)\nmas tentaram escrever nela.", env->programCounter);

    // Se chegou aqui, a memória ainda está no intervalo

    // Verifica se tem algo nela (para avisar o usuário que ele
    // está sobrescrevendo algo)
    if (isAddressUsed(env, env->programCounter) || isAddressUsed(env, env->programCounter+1))
        WARN("A posicao de memoria \"%x\" vai ser sobrescrita, mas ha conteudo nela.\nIsso pode causar comportamentos inesperados.", env->programCounter);

    // Escreve o LSB
    env->memory[env->programCounter] = (memoryUnit_t) {
        .value = (hex1_t)(hex & 0xFF),
        .annotation = EMPTY_ANNOTATION
    };
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;

    // Escreve o MSB
    env->memory[env->programCounter] = env->memory[env->programCounter] = (memoryUnit_t) {
        .value = (hex1_t)(hex >> 8),
        .annotation = EMPTY_ANNOTATION
    };
    addAddressToUsedMemory(env, env->programCounter);
    env->programCounter++;
}


int getLabelFromAddress(Environment * env, uhex2_t address) {
    // Procura o endereço
    for (int i = 0; i < env->symbolCount; i++) {
        if (env->symbolTable[i].value == address)
            return i;
    }

    return -1;
}

int getLabelFromName(Environment * env, char * name) {
    // Procura o endereço
    for (int i = 0; i < env->symbolCount; i++) {
        if (strcmp(env->symbolTable[i].name, name) == 0)
            return i;
    }

    return -1;
}

void addLabel(Environment * env, char * name, uhex2_t address) {
    if (!env->isFirstPass) return;
    // Verifica se já não existe esse nome.
    int i = getLabelFromName(env, name);
    if (i != -1)
        WARN("Instrucao %d: ja existe um rotulo com o nome \"%s\".\n\t O novo rotulo ira sobrescrever o antigo.", env->currentInstruction, name);

    // Salva esse endereço na Tabela de Símbolos com o nome dado
    label_t label = {
        .name = name,
        .value = address
    };

    // Se encontrou um com o mesmo nome, sobrescreve
    if (i != -1) {
        env->symbolTable[i] = label;
        return;
    }

    // Se chegou até aqui, não encontrou um rótulo com o mesmo nome,
    // então cria um novo
    env->symbolCount++;
    label_t * temp = realloc(env->symbolTable, sizeof(label_t) * env->symbolCount);
    if (temp == NULL)
        V_EXIT(EXIT_NO_MEMORY,"Nao ha memoria suficiente para salvar o rotulo \"%s\"", name);

    temp[env->symbolCount - 1] = label;

    env->symbolTable = temp; // salva o novo vetor
}

uhex2_t getValueOfLabel(Environment * env, char * name) {
    if (env->isFirstPass) {
        return 0;
    }

    // Procura o rótulo
    int label = getLabelFromName(env, name);
    if (label == -1) // se não encontrou, dá erro
        V_EXIT(EXIT_INVALID_ARGUMENT, "Instrucao %d: O rotulo \"%s\" nao existe. ", env->currentInstruction, name);

    // Se chegou até aqui, encontrou, então retorna o endereço.
    return env->symbolTable[label].value;
}


void addInstruction(Environment * env, uhex1_t opcode) {
    addToMemoryHex1Annotation(env, opcode, (char*)getInstructionName(opcode));
}

void addInstructionWithHex1(Environment * env, uhex1_t opcode, hex1_t value) {
    addToMemoryHex1Annotation(env, opcode,
        formatString("%s %xH", (char*)getInstructionName(opcode), value)
        );
    addToMemoryHex1(env, value);
}

void addInstructionWithHex2(Environment * env, uhex1_t opcode, hex2_t value) {
    if (opcode == OPCODE_JMP || opcode == OPCODE_JM || opcode == OPCODE_JNZ || opcode == OPCODE_JZ || opcode == OPCODE_CALL) {
        int i = getLabelFromAddress(env, value);
        if (i != -1) {
            addToMemoryHex1Annotation(env, opcode,
        formatString("%s %s\t\t(%s aponta para %xH)",
            (char*)getInstructionName(opcode),
            env->symbolTable[i].name,
            env->symbolTable[i].name,
            (uhex2_t)value)
        );
        } else {
            addToMemoryHex1Annotation(env, opcode,
        formatString("%s %x", (char*)getInstructionName(opcode), value)
        );
        }

    } else {
        addToMemoryHex1Annotation(env, opcode,
        formatString("%s %xH", (char*)getInstructionName(opcode), value)
        );
    }

    addToMemoryHex2(env, value);
}

void appendAnnotationToLastMemoryUnit(Environment * env, char * text) {
    if (env->isFirstPass) return;
    env->memory[env->programCounter-1].annotation = formatString("%s, %s",
        env->memory[env->programCounter-1].annotation, text);
}

void setRegister(Environment * env, int reg, hex1_t value) {
    // Altera o valor do registrador
    env->registers[reg] = value;

    // Atualiza os flags. Teoricamente, os flags só seriam atualizados
    // se fosse o acumulador que tivesse alterado. No entanto, não achei
    // nenhum lugar que confirmasse isso e os próprios códigos
    // disponibilizados dependem que os flags alterem na mudança de qualquer
    // registrador.

 //   if (reg == ACCUMULATOR) {
        // Redefine os flags
        env->flags[FLAG_Z] = 0;
        env->flags[FLAG_S] = 0;

        // Se for negativo, ativa o flag de sinal
        if (value < 0) env->flags[FLAG_S] = 1;

        // Se for 0, ativa o flag de zerado
        else if (value == 0) env->flags[FLAG_Z] = 1;
 //   }
}

void setMemory(Environment * env, uhex2_t address, hex1_t value) {
    if (isAddressUsed(env, address)) {
        char* comp;
        if (strcmp(env->memory[address].annotation,EMPTY_ANNOTATION) != 0) {
            comp = formatString(
                "Antes: %02xH\t(Anotacao: %s)\n\tDepois: %02xH\t(Anotacao: %s)",
                (uhex1_t)env->memory[address].value,
                env->memory[address].annotation,
                (uhex1_t)value,
                EVAL_DEFINED_MEMORY_ANNOTATION);
        } else {
            comp = formatString(
                "Antes: %02xH\n\tDepois: %02xH(Anotacao: %s)",
                (uhex1_t)env->memory[address].value,
                (uhex1_t)value,
                EVAL_DEFINED_MEMORY_ANNOTATION);
        }
        WARN(
            "O endereco \"%4x\" da memoria esta sendo sobrescrito.\n\t%s",
            address,
            comp);
    }

    env->memory[address].value = value;
    env->memory[address].annotation = EVAL_DEFINED_MEMORY_ANNOTATION;
}

void setMemoryHex2(Environment * env, uhex2_t address, hex2_t value) {
    hex1_t lsb = (hex1_t) (value & 0xFF);
    hex1_t msb = (hex1_t) ((value >> 8) & 0xFF);

    setMemory(env, address, lsb);
    setMemory(env, address+1, msb);
}

void setMemoryWithAnnotation(Environment * env, uhex2_t address, hex1_t value, const char * annotation) {
    env->memory[address].value = value;
    env->memory[address].annotation = strdup(annotation);
}

// env->memory[address]
#define env_memory(address) env->memory[address]
// env->memory[address].value
#define env_memval(address) env_memory(address).value

void print_memory(Environment * env) {
    printf("\nMemoria RAM ================================\nEndereco\t| Conteudo\t| Simbolico\n");

    qsort(env->usedAddresses, env->usedAddressesSize, sizeof(hex2_t), comp_hex2);

    for (size_t i = 0; i < env->usedAddressesSize; i++) {
        char* annotation = env_memory(env->usedAddresses[i]).annotation;
        // é instrução
        printf("%xH\t\t| %02xH \t\t| %s\n",
        env->usedAddresses[i],
        // Se for instrução, o valor guardado deverá ser lido como
        // unsigned hex.
        (uhex1_t)env_memval(env->usedAddresses[i]),
        env_memory(env->usedAddresses[i]).annotation);
    }
    printf("\n");
}

void print_flags(Environment * env) {
    printf("Flags ================\nFlag      | Valor\n");
    printf("S (Sinal) | %d\n", env->flags[FLAG_S]);
    printf("Z (Zero)  | %d\n\n", env->flags[FLAG_Z]);
}

void print_regs(Environment * env) {
    printf("Registradores ========\nRegistrador    | Valor\n");
    printf("A (Acumulador) | %xH\n", env->registers[ACCUMULATOR]);
    printf("B              | %xH\n", env->registers[REGISTER_B]);
    printf("C              | %xH\n", env->registers[REGISTER_C]);

}


void print_info(Environment * env) {
    print_memory(env);
    print_flags(env);
    printf("Quantidade de instrucoes executadas: %u\n", env->currentInstruction);
}

void print_debug_info(Environment * env) {
    printf("Quantidade de instrucoes executadas: %u\n", env->currentInstruction-1);
    print_regs(env);
    print_flags(env);
}

hex1_t get_1hex_from_in(uhex1_t flow) {
    char in[MAX_LENGTH_SINGLE_HEX + 1];
    if (_in_flow(flow) == stdinflow)
        printf("\nEntrada atual:");

    fgets(in, MAX_LENGTH_SINGLE_HEX + 1, _in_flow(flow));
    for (int i = MAX_LENGTH_SINGLE_HEX; i > 0; i--) {
        if (in[i] == 'H' || isalpha(in[i])) break;
        in[i] = '\0';
    }

    // Transforma o texto em um hexadecimal e verifica se teve erros
    hex1_t temp;
    errno_t c_exit = str_to_hex1(in, &temp);
    switch (c_exit) {
        case EXIT_SUCCESS: break;
        case EXIT_INVALID_ARGUMENT:
            V_EXIT(EXIT_INVALID_ARGUMENT,
                "A instrucao IN esperava um hexadecimal, mas recebeu \"%s\".\nVerifique se o numero esta na forma 12H.", in);
        case EXIT_NULL_ARGUMENT:
            EXIT_CUSTOM_ERR(EXIT_NULL_ARGUMENT,
                "A instrucao IN esperava um hexadecimal, mas recebeu nada.\nVerifique se o numero foi digitado corretamente e esta na forma 12H.");

        default: {
            V_EXIT(EXIT_INVALID_ARGUMENT,
                "A instrucao IN esperava um hexadecimal, mas recebeu um termo desconhecido: \"%s\"", in);
        }
    }

    // Retorna o hexadecimal obtido
    return temp;
}