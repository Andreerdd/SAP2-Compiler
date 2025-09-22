// Analisa e avalia as instruções que o usuário deu.
//
// Author: André
// Date: 19/09/2025
//

#include <stdbool.h>
#include <stdint.h> // strtout (char* -> hex)
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "../ErrorCodes.h"
#include "../Utils/Utils.h"
#include "../Instructions/Instructions.h"

// Se o texto da instrução for o valor dado, analisa a instrução
#define PARSE_IF_INSTRUCTION(I) \
    else if (strcmp(t_instruction->value, #I) == 0) \
    parse_##I(state);

// O ambiente do estado atual do parser
#define stenv (state->env)

// Verifica se o valor v é um hexadecimal dentro dos limites de "s" hexadecimais (4*s bits).
#define check_hex(name, s, v) \
    uhex##s##_t name;                                                                       \
    if (CAT2(str_to_hex, s)(v->value, &name) != EXIT_SUCCESS)                                     \
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Argumento hexadecimal invalido \"%s\" (%s)\n",      \
        v ? v->value : "null",                                                              \
        v ? getTokenTypeString(v->type) : getTokenTypeString(TokenType_Unknown));           \
    if (name > UHEX##s##_MAX)                                                               \
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Valor hexadecimal muito grande: %x\n", name);

// Adiciona à memória a respectiva instrução "inst" em função do registrador "ri"
#define addInstructionAllReg(inst, ri) \
    do {    \
        if (ri == ACCUMULATOR) addToMemoryHex1(stenv, inst##_A); \
        else if (ri == REGISTER_B) addToMemoryHex1(stenv, inst##_B); \
        else if (ri == REGISTER_C) addToMemoryHex1(stenv, inst##_C); \
    } while (0);

#define parsei(name) \
    void parse_##name(ParserState * state)

typedef struct {
    // Ambiente do SAP2
    Environment * env;
    // Array dos tokens
    Token_t * tokens;
    // Tamanho do array dos tokens
    size_t size;
    // Posição atual no array dos tokens
    size_t index;
} ParserState;

// Retorna o token atual sem avançar
Token_t* peek(ParserState* state) {
    if (state->index >= state->size) return NULL;
    return &state->tokens[state->index];
}


// Retorna o próximo token, sem avançar o estado atual.
Token_t* peek_i(ParserState* state, size_t i) {
    // Verifica se há um próximo token para espiar
    if (state->index + i >= state->size) {
        return NULL; // Chegamos ao fim, não há próximo token
    }
    // Retorna o token na posição atual + i
    return &state->tokens[state->index + i];
}

// Avança para o próximo token
void consume(ParserState* state) {
    if (state->index < state->size) {
        state->index++;
    }
}

// Avança para o próximo token apenas se o atual for o esperado
Token_t* expect_and_consume(ParserState* state, TokenType_t expected_type) {
    Token_t* token = peek(state);
    if (token == NULL || token->type != expected_type) {
        // Avisa o erro e sai do programa
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Erro de sintaxe: esperado %s, encontrado \"%s\" (%s)\n",
            getTokenTypeString(expected_type),
            token ? token->value : "null",
            token ? getTokenTypeString(token->type) : getTokenTypeString(TokenType_Unknown));

        return NULL;
    }
    consume(state);
    return token;
}

/**
 * Consome um registrador.
 * @param state o estado do parser
 * @return o índice do acumulador no array de registradores
 */
int consume_reg(ParserState * state) {
    Token_t * temp = expect_and_consume(state, TokenType_Register);
    if (temp == NULL || temp->value == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Registrador invalido.\n");

    int ri = charToRegister(temp->value[0]);
    if (ri == -1)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Registrador invalido: \"%c\"\n", temp->value[0]);

    return ri;
}

/**
 * Consome um hexadecimal.
 * @param state estado do parser
 * @param size quantos hexadecimais (0xFF) deve consumir
 * @return o hexadecimal
 */
int consume_hex(ParserState * state, unsigned int size) {
    Token_t * temp = expect_and_consume(state, TokenType_Hexadecimal);

    if (size == 1) {
        check_hex(rx, 1, temp);
        return rx;
    }

    // se o tamanho não é 1, só pode ser 2
    check_hex(rx, 2, temp);
    return rx;
}

parsei(ADD) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addToMemoryHex1(stenv, OPCODE_ADD_B);
    else if (ri == REGISTER_C) addToMemoryHex1(stenv, OPCODE_ADD_C);
}

parsei(DCR) {
    int ri = consume_reg(state);

    addInstructionAllReg(OPCODE_DCR, ri);
}

parsei(HLT) {
    // Antes de sair, verifica se tem coisa depois
    Token_t * t_end = peek(state);

    if (t_end != NULL && t_end->type != TokenType_EOF) {
        I_WARN("%s", "Coisas depois do HLT foram descartadas.");
    }

    addToMemoryHex1(stenv, OPCODE_HLT);
}

parsei(MVI) {
    int ri = consume_reg(state);

    expect_and_consume(state, TokenType_Comma);

    uhex1_t xv = consume_hex(state, 1);

    addInstructionAllReg(OPCODE_MVI, ri);
    addToMemoryHex1(state->env, xv);
}

parsei(OUT) {
    uhex2_t xv = consume_hex(state, 2);

    addToMemoryHex1(stenv, OPCODE_OUT);
    addToMemoryHex2(stenv, xv);
}

parsei(SUB) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addToMemoryHex1(stenv, OPCODE_SUB_B);
    else if (ri == REGISTER_C) addToMemoryHex1(stenv, OPCODE_SUB_C);
}


// analisa uma instrução
void parse_instruction(ParserState * state) {
    Token_t * t_instruction = expect_and_consume(state, TokenType_Instruction);

    if (strcmp(t_instruction->value, "HLT") == 0) {
        parse_HLT(state);
    }
    PARSE_IF_INSTRUCTION(ADD)
    PARSE_IF_INSTRUCTION(DCR)
    PARSE_IF_INSTRUCTION(MVI)
    PARSE_IF_INSTRUCTION(OUT)
    PARSE_IF_INSTRUCTION(SUB)

    state->env->currentInstruction++;
}

// analisa o trecho de tokens atual
bool parse_statement(ParserState * state) {
    Token_t * token = peek(state);
    if (token == NULL) return false;
    switch (token->type) {
        case TokenType_Instruction: {
            parse_instruction(state);
            return false;
        }
        case TokenType_EOF: return true;

        default: VI_EXIT(EXIT_INVALID_INSTRUCTION, "Nao foi possivel analisar o trecho: %s", token->value);
    }
}

void parse(Token_t * tokens, size_t size, Environment * env) {
    ParserState state = {
        .env = env,
        .tokens = tokens,
        .size = size,
        .index = 0
    };

    while (state.index < state.size) {
        if (parse_statement(&state)) {
            return;
        };
    }
}