#include <stdio.h>

#include "Interpreter/ErrorCodes.h"
#include "Interpreter/interpreter.h"

#define DEBUG_FILE "../test.asm"


int main(int argc, char ** argv) {
    //if (argc <= 1)
    //    RETURN_ERR(EXIT_NO_FILE);

    //FILE * file = fopen(argv[1], "r");
    FILE * file = fopen(DEBUG_FILE, "r");
    if (file == NULL)
        RETURN_ERR(EXIT_FILE_NOT_FOUND);

    return interpret(file);
}
