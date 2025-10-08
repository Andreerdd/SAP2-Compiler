cmake_minimum_required(VERSION 4.0)
project(SAP2_Compiler C)

set(CMAKE_C_STANDARD 11)

add_executable(SAP2_Compiler main.c
        Interpreter/Analysis/tokenizer.c
        Interpreter/Analysis/tokenizer.h
        Interpreter/ErrorCodes.h
        Interpreter/Utils/Utils.h
        Interpreter/Utils/Utils.c
        Interpreter/interpreter.c
        Interpreter/interpreter.h
        Interpreter/Analysis/parser.c
        Interpreter/Analysis/parser.h
        Interpreter/environment.c
        Interpreter/environment.h
        Interpreter/Instructions/InstructionsFunctions.h
        Interpreter/Instructions/InstructionsFunctions.c
        Interpreter/Instructions/InstructionsFunctions.h
        Interpreter/Instructions/Instructions.c
        Interpreter/Runtime/evaluate.c
        Interpreter/Runtime/evaluate.h)
