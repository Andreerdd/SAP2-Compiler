//
//
// Author: André
// Date: 19/09/2025
//
#pragma once

#ifndef SAP2_COMPILER_INTERPRETER_H
#define SAP2_COMPILER_INTERPRETER_H

#include <stdio.h>
#include "Utils/Utils.h"
#include "ErrorCodes.h"
#include "environment.h"

// Interpreta o arquivo
ErrorCode_t interpret(FILE * file, Parametros * parametros);

Parametros * get_standard_parameters();

#endif //SAP2_COMPILER_INTERPRETER_H