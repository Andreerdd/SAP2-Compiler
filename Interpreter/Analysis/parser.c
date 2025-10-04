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

// TODO: avisar que o hexadecimal é de um byte (hex1) ou de dois bytes (hex2) na mensagem de erro

// Verifica se o valor v é um hexadecimal dentro dos limites de "s" hexadecimais (4*s bits).
#define check_hex(name, s, v) \
    hex##s##_t name;                                                                       \
    if (CAT2(str_to_hex, s)(v->value, &name) != EXIT_SUCCESS)                                     \
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Argumento hexadecimal invalido \"%s\" (%s)\n",      \
        v ? v->value : "null",                                                              \
        v ? getTokenTypeString(v->type) : getTokenTypeString(TokenType_Unknown));           \
    if (name > HEX##s##_MAX)                                                               \
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Valor hexadecimal muito grande: %x\n", name);

// Adiciona à memória a respectiva instrução "inst" em função do registrador "ri"
#define addInstructionAllReg(inst, ri) \
    do {    \
        if (ri == ACCUMULATOR) addInstruction(stenv, inst##_A); \
        else if (ri == REGISTER_B) addInstruction(stenv, inst##_B); \
        else if (ri == REGISTER_C) addInstruction(stenv, inst##_C); \
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
        Token_t last_token;
        state->index--;
        do {
            last_token = state->tokens[state->index];
            if (state->index == 0) {
                // Avisa o erro e sai do programa
                VI_EXIT(EXIT_INVALID_ARGUMENT, "Erro de sintaxe: esperado %s, encontrado \"%s\" (%s)\n",
                getTokenTypeString(expected_type),
                token ? token->value : "null",
                token ? getTokenTypeString(token->type) : getTokenTypeString(TokenType_Unknown));
            }
            state->index--;
        } while (last_token.type != TokenType_Identifier && last_token.type != TokenType_Instruction);

        // Avisa o erro e sai do programa
        VI_EXIT(EXIT_INVALID_ARGUMENT, "Erro de sintaxe depois de \"%s\": esperado %s, encontrado \"%s\" (%s)\n",
        last_token.value,
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
hex2_t consume_hex(ParserState * state, unsigned int size) {
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

    if (ri == REGISTER_B) addInstruction(stenv, OPCODE_ADD_B);
    else if (ri == REGISTER_C) addInstruction(stenv, OPCODE_ADD_C);
}

parsei(CALL) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_CALL, address);
}

parsei(RET) {
    addInstruction(stenv, OPCODE_RET);
}

parsei(NOP) {
    addInstruction(stenv, OPCODE_NOP);
}

parsei(DCR) {
    int ri = consume_reg(state);

    addInstructionAllReg(OPCODE_DCR, ri);
}

parsei(HLT) {
    /* Vou melhorar isso depois :D

    // Antes de sair, verifica se tem coisa depois
    Token_t * t_end = peek(state);

    if (t_end != NULL && t_end->type != TokenType_EOF) {
        I_WARN("%s", "Coisas depois do HLT foram descartadas.");
    }
    */
    addInstruction(stenv, OPCODE_HLT);
}

parsei(JMP) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_JMP, address);
}

parsei(JM) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_JM, address);
}

parsei(JNZ) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_JNZ, address);
}

parsei(JZ) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_JZ, address);
}

parsei(MOV) {
    #define opc_mov(x, y) case REGISTER_##y: {addInstruction(stenv, OPCODE_MOV_##x##_##y); break;}

    int ri = consume_reg(state);
    expect_and_consume(state, TokenType_Comma);
    int rj = consume_reg(state);

    switch (ri) {
    case ACCUMULATOR:
        switch (rj) {
            opc_mov(A, B)
            opc_mov(A, C)
            default: {}
        }
        break;
    case REGISTER_B:
        switch (rj) {
            opc_mov(B, A)
            opc_mov(B, C)
            default: {}
        }
        break;
    case REGISTER_C:
        switch (rj) {
            opc_mov(C, A)
            opc_mov(C, B)
            default: {}
        }
        break;
    default: {}
    }
}

parsei(MVI) {
    int ri = consume_reg(state);

    expect_and_consume(state, TokenType_Comma);

    hex1_t xv = (hex1_t)consume_hex(state, 1);

    addInstructionAllReg(OPCODE_MVI, ri);
    appendAnnotationToLastMemoryUnit(stenv, toStringUHex((uhex1_t)xv));
    addToMemoryHex1(state->env, xv);
}

parsei(OUT) {
    hex2_t xv = consume_hex(state, 2);

    addInstructionWithHex2(stenv, OPCODE_OUT, xv);
}

parsei(SUB) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addInstruction(stenv, OPCODE_SUB_B);
    else if (ri == REGISTER_C) addInstruction(stenv, OPCODE_SUB_C);
}

parsei(ANA) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addInstruction(stenv, OPCODE_ANA_B);
    else if (ri == REGISTER_C) addInstruction(stenv, OPCODE_ANA_C);
}

parsei(ANI) {
    hex1_t xv = (hex1_t)consume_hex(state, 1);
    addInstructionWithHex1(stenv, OPCODE_ANI, xv);
}

parsei(CMA) {
    addInstruction(stenv, OPCODE_CMA);
}

parsei(INR) {
    int ri = consume_reg(state);
    addInstructionAllReg(OPCODE_INR, ri);
}

parsei(LDA) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_LDA, address);
}

parsei(ORA) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addInstruction(stenv, OPCODE_ORA_B);
    else if (ri == REGISTER_C) addInstruction(stenv, OPCODE_ORA_C);
}

parsei(ORI) {
    hex1_t xv = (hex1_t)consume_hex(state, 1);
    addInstructionWithHex1(stenv, OPCODE_ORI, xv);
}

parsei(RAL) {
    addInstruction(stenv, OPCODE_RAL);
}

parsei(RAR) {
    addInstruction(stenv, OPCODE_RAR);
}

parsei(STA) {
    Token_t * t_address = peek(state);
    uhex2_t address;
    if (t_address == NULL)
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria (hexadecimal).");

    if (t_address->type == TokenType_Hexadecimal) {
        address = consume_hex(state, 2);
    } else if (t_address->type == TokenType_Identifier) {
        consume(state);
        address = getAddressOfLabel(stenv, t_address->value);
    } else {
        VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "Esperado rotulo ou endereco de memoria(hexadecimal).");
    }
    addInstructionWithHex2(stenv, OPCODE_STA, address);
}

parsei(XRA) {
    int ri = consume_reg(state);

    if (ri == REGISTER_B) addInstruction(stenv, OPCODE_XRA_B);
    else if (ri == REGISTER_C) addInstruction(stenv, OPCODE_XRA_C);
}

parsei(XRI) {
    hex1_t xv = (hex1_t)consume_hex(state, 1);
    addInstructionWithHex1(stenv, OPCODE_XRI, xv);
}



// analisa uma instrução
void parse_instruction(ParserState * state) {
    Token_t * t_instruction = expect_and_consume(state, TokenType_Instruction);

    if (strcmp(t_instruction->value, "HLT") == 0) {
        parse_HLT(state);
    }
    PARSE_IF_INSTRUCTION(ADD)
    PARSE_IF_INSTRUCTION(CALL)
    PARSE_IF_INSTRUCTION(DCR)
    PARSE_IF_INSTRUCTION(JMP)
    PARSE_IF_INSTRUCTION(JM)
    PARSE_IF_INSTRUCTION(JNZ)
    PARSE_IF_INSTRUCTION(JZ)
    PARSE_IF_INSTRUCTION(MOV)
    PARSE_IF_INSTRUCTION(MVI)
    PARSE_IF_INSTRUCTION(NOP)
    PARSE_IF_INSTRUCTION(OUT)
    PARSE_IF_INSTRUCTION(RET)
    PARSE_IF_INSTRUCTION(SUB)
    PARSE_IF_INSTRUCTION(ANA)
    PARSE_IF_INSTRUCTION(ANI)
    PARSE_IF_INSTRUCTION(CMA)
    PARSE_IF_INSTRUCTION(INR)
    PARSE_IF_INSTRUCTION(LDA)
    PARSE_IF_INSTRUCTION(ORA)
    PARSE_IF_INSTRUCTION(ORI)
    PARSE_IF_INSTRUCTION(RAL)
    PARSE_IF_INSTRUCTION(RAR)
    PARSE_IF_INSTRUCTION(STA)
    PARSE_IF_INSTRUCTION(XRA)
    PARSE_IF_INSTRUCTION(XRI)

    state->env->currentInstruction++;
}

// analisa um identificador
void parse_identifier(ParserState * state) {
    Token_t * identifier_token = expect_and_consume(state, TokenType_Identifier);
    char* lname = strdup(identifier_token->value);
    if (lname == NULL) VI_EXIT(EXIT_INVALID_ARGUMENT, "%s", "O rotulo eh invalido");

    (void) expect_and_consume(state, TokenType_Colon);

    // Salva esse endereço na Tabela de Símbolos com o nome dado
    addLabel(state->env, lname, state->env->programCounter);
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
        case TokenType_Identifier: {
            parse_identifier(state);
            return false;
        }

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

    env->isFirstPass = true;
    while (state.index < state.size) {
        if (parse_statement(&state)) {
            break;
        };
    }

    env->isFirstPass = false;
    state.index = 0;
    env->programCounter = STARTER_MEMORY_ADDRESS;
    env->currentInstruction = 0;
    while (state.index < state.size) {
        if (parse_statement(&state)) {
            return;
        };
    }

}