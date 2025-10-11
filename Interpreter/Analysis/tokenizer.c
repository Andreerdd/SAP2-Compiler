// Transforma as palavras do arquivo em tokens
// para facilitar a interpretação do código.
//
// Author: André
// Date: 16/09/2025
//


#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer.h"
#include "../Utils/Utils.h"
#include "../Instructions/Instructions.h"

/**
 * Retorna se o caractere é considerado como "alfa".
 * Essa função é usada, principalmente, para definir se um
 * caractere pode ser usado em um identificador.
 * @param c caractere que se quer verificar
 * @return se é válido ou não
 */
bool validCharForAlpha(char c) {
    if (isalpha(c)) return true;
    static const char valid[] = {
        '_'
    };
    for (int i = 0; i < sizeof(valid)/sizeof(char); i++) {
        if (valid[i] == c) return true;
    }
    return false;
}

/**
 * Retorna o tipo de token que o texto é.
 * @param text o texto que se quer saber o tipo de token
 * @return o tipo de token
 */
TokenType_t getTypeOfToken(char* text) {
    size_t len = strlen(text);

    // Se não tiver tamanho, retorna desconhecido
    if (len <= 0) return TokenType_Unknown;

    if (len == 1)
        switch (text[0]) {
            case 'A':
            case 'B':
            case 'C': return TokenType_Register;
            case '\n':
            case '\r':
            case '\t': return TokenType_Space;
            case ';': return TokenType_Semicolon;
            case ':': return TokenType_Colon;
            case EOF: return TokenType_EOF;
            case ' ': return TokenType_Space;
            default: break;
        }

    // Para char e string
    /*unsigned int quoted = 0;       // a quantidade de aspas entre o texto inteiro
    if (text[0] == '\'' && text[len-1] == '\'') quoted = 1;
    else if (text[0] == '\"' && text[len-1] == '\"') quoted = 2;

    if (quoted == 1) {
        return TT_LITERAL_CHAR;
    } else if (quoted == 2) {
        return TT_LITERAL_STRING;
    }*/

    unsigned char startsWithNumber = 0;
    if (isdigit(text[0])) startsWithNumber = 1;

    bool endsWithH = false;
    if (text[len-1] == 'H') endsWithH = true;

    unsigned int qntdDigit = 0;     // quantidade de números
    unsigned int qntdAlpha = 0;     // quantidade de letras
    unsigned int qntdHex = 0;       // quantidade de caracteres hexadecimais

    unsigned int qntdDot = 0;       // quantidade de pontos
    for (int i = 0; i < len; i++) {
        if (isdigit(text[i])) qntdDigit++;
        if (isxdigit(text[i])) qntdHex++;
        if (validCharForAlpha(text[i])) qntdAlpha++;

        if (text[i] == '.') qntdDot++;
    }

    // se todos os caracteres forem algarismos (ou se
    // houver um ponto entre os números), avisa o
    // usuário que o programa só aceita hexadecimal.
    if ( (qntdDigit == len) || (qntdDigit == len-1 && qntdDot == 1) ) {
        V_EXIT(EXIT_INVALID_ARGUMENT,
    "O numero \"%s\" foi encontrado.\nNo entanto, apenas hexadecimais sao permitidos.\nNesse programa, hexadecimais sao numeros inteiros\nque terminam com H, como 1234H.",
    text);
    }

    // Se todos os caracteres menos um for um digito
    // e o último caractere for um H (ou seja, for um
    // número seguido de um H), é um hexadecimal
    if (qntdHex == len-1 && endsWithH) {
        return TokenType_Hexadecimal;
    }
    else if (!startsWithNumber && qntdDigit + qntdAlpha == len) {
        return isInstruction(text) ? TokenType_Instruction : TokenType_Identifier;
    }
    return TokenType_Unknown;
}

char* getTokenTypeString(TokenType_t type) {
    switch (type) {
        case TokenType_Register: return "Registador";
        case TokenType_Hexadecimal: return "Hexadecimal";
        case TokenType_Identifier: return "Identificador";
        case TokenType_Instruction: return "Palavra Chave";
        case TokenType_Semicolon: return "Ponto e virgula";
        case TokenType_Comma: return "Virgula";
        case TokenType_EOF: return "EOF";
        case TokenType_Space: return "Espaco";
        case TokenType_Colon: return "Dois pontos";
        case TokenType_Unknown: return "Desconhecido";
        default: {
            WARN("Tipo de token indefinido. Valor enum: %d", type);
            return "Desconhecido";
        }
    }
}

/**
 * Adiciona o token no fim do array de tokens
 * @param token o token que se quer ser adicionado
 * @param array o array que se quer adicionar o token
 * @param size  o tamanho do vetor de token
 * @return o novo tamanho do vetor de tokens
 */
size_t addTokenToArray(Token_t token, Token_t** array, size_t size) {
    Token_t* tmp = realloc(*array, (size + 1) * sizeof(Token_t));
    if (tmp == NULL)
        return -1;
    *array = tmp;

    (*array)[size] = token;
    return size + 1;
}

/**
 * Constrói um token a partir dos valores dados
 * @param type tipo do token
 * @param value texto do token
 * @return o token construído com os valores dados
 */
Token_t buildToken(TokenType_t type, const char* value) {
    // Clona o texto
    char* copy = NULL;
    if (value != NULL) {
        copy = strdup(value);
        if (copy == NULL) {
            EXIT_ERR(EXIT_NO_MEMORY);
            /*return (Token_t) {
                .type = TokenType_Unknown,
                .value = NULL
            };*/
        }
    }

    // Cria o token e retorna
    Token_t token = (Token_t) {
        .type = type,
        .value = copy
    };
    return token;
}


Token_t getNextToken(FILE* file) {

    // Inicializa a string do texto
    char* text = NULL;
    size_t textSize = 0;
    bool comment = false;

    // Lê os caracteres do arquivo até encontrar algo que
    // "separa" tokens
    char c;
    do {
        // Obtém o próximo caractere
        c = (char)fgetc(file);

        // Se chegou no fim do arquivo
        if (c == EOF) {
            // Se não leu nada de texto, retorna um EOF
            if (text == NULL)
                return buildToken(TokenType_EOF, "EOF");

            // Se leu alguma coisa, acaba o loop.
            // Um "HLT" no fim do arquivo chegaria aqui, por exemplo.
            break;
        }
        // Se chegou até aqui, não chegou no fim do arquivo

        // Verifica se é o início de um comentário
        if (c == COMMENT_CHAR) {
            comment = true;
            continue;
        }
        // Verifica se está em comentário
        if (comment) {
            // Se encontrou uma quebra de linha, para o comentário
            if (c == '\n') {
                comment = false;
                if (text != NULL)
                    break;

            }
            continue;
        }

        // Se é um espaço, \n, \t ou vírgula, acaba o token ou continua
        if (c == ' ' || c == '\n' || c == '\r' || c == '\t') {
            // Se não leu nada, continua a ler
            if (text == NULL)
                continue;

            // Se chegou aqui, leu alguma coisa, então para
            break;

        }

        // Se for uma vírgula, salva o texto já lido
        // e volta 1 caractere no arquivo
        if (c == ',' || c == ':') {
            if (text != NULL) {
                // Volta 1 caractere
                if (ungetc(c, file) == EOF) {
                    // Se não conseguir, é, necessariamente, por falta de
                    // memória, uma vez que ungetc só retorna EOF se o
                    // caractere dado for EOF(o que não é no nosso caso)
                    // ou se não houver espaço no buffer de push-back.
                    // Nesse caso, só sai do programa.
                    EXIT_ERR(EXIT_NO_MEMORY);
                }
                // Para o loop para salvar o texto lido
                break;
            }
            // Se chegou até aqui (não tem texto para salvar),
            // cria um token de vírgula/dois pontos
            if (c == ',') return buildToken(TokenType_Comma, ",");
            else return buildToken(TokenType_Colon, ":");
        }

        int alpha = isalpha(c); // se é um caractere do alfabeto
        int digit = isdigit(c); // se é um número

        // Se é uma letra ou um número, adiciona na string
        if ( alpha || digit ) {
            textSize = addCharToString(c, &text, textSize);
            if (textSize == -1) {
                EXIT_ERR(EXIT_NO_MEMORY);
                //return buildToken(TokenType_Unknown, NULL);
            }
        } else {
            // Se o caractere for desconhecido, imprime uma mensagem
            // e pula ele
            WARN("Caractere invalido: %c", c);
            continue;
        }
    } while (c);

    // Constrói e retorna o token, além de liberar memória
    Token_t token = buildToken(getTypeOfToken(text), text);
    free(text);
    return token;
}

ErrorCode_t tokenize(FILE* file, Token_t** finalTokenArray, size_t* finalSize) {
    // Se algum dos parâmetros for nulo (não deve acontecer, já
    // que há verificação antes)
    if (file == NULL)
        RETURN_ERR(EXIT_FILE_NOT_FOUND);
    if (finalTokenArray == NULL)
        RETURN_CUSTOM_ERR(EXIT_INVALID_ARGUMENT, "Erro interno: o array de tokens eh null");
    if (finalSize == NULL)
        RETURN_CUSTOM_ERR(EXIT_INVALID_ARGUMENT, "Erro interno: o endereco do tamanho do array de tokens eh null");

    // Inicializa o vetor de tokens
    Token_t* tokens = NULL;
    size_t size = 0;

    // O token atual
    Token_t currToken;

    // Obtém os tokens do arquivo
    do {
        currToken = getNextToken(file);
        // Adiciona o token ao array. Se não conseguir, para
        // de interpretar
        size = addTokenToArray(currToken, &tokens, size);
        if (size == -1)
            RETURN_ERR(EXIT_NO_MEMORY);
    } while (currToken.type != TokenType_EOF);

    *finalTokenArray = tokens;
    *finalSize = size;

    return EXIT_SUCCESS;
}
