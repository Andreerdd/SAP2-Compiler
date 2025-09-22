// Executa as instruções que estão na memória
//
// Author: André
// Date: 21/09/2025
//

#ifndef SAP2_COMPILER_EVALUATE_H
#define SAP2_COMPILER_EVALUATE_H

#include "../Instructions/Instructions.h"
#include "../environment.h"
#include "../ErrorCodes.h"

/**
 * Executa as instruções na memória do SAP2
 * @param env o ambiente do SAP2
 * @return o código de erro
 */
ErrorCode_t evaluate(Environment * env);

#endif //SAP2_COMPILER_EVALUATE_H