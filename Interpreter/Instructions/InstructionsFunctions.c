//
//
// Author: André
// Date: 19/09/2025
//

#include <stdio.h>
#include <stdlib.h>

#include "InstructionsFunctions.h"
#include "../Utils/Utils.h"
#include "../ErrorCodes.h"


// env->registers[r]
#define REG(r) env->registers[r]

// env->registers[ACCUMULATOR]
#define REG_A REG(ACCUMULATOR)

ex_fn_reg(execute_add) {
    REG_A += env->registers[reg];
}

ex_fn_reg(execute_dcr) {
    REG(reg)--;
}

ex_fn(execute_hlt) {
    // Antes de sair, imprime a memória

    // Esse if é praticamente desnecessário, já que, para chegar aqui,
    // precisaria de ter o OPCODE do HLT na memória (portanto, algum
    // endereço da memória foi usado).
    if (env->usedAddressesSize > 0 && SHOW_MEMORY_ON_HLT) {
        printf("\nMemoria RAM ====================\nEndereco\t|\tConteudo\n");

        qsort(env->usedAddresses, env->usedAddressesSize, sizeof(uhex2_t), comp_hex2);

        for (size_t i = 0; i < env->usedAddressesSize; i++) {
            printf("%xH\t\t|\t%xH\n", env->usedAddresses[i], env->memory[env->usedAddresses[i]]);
        }
    }

    exit(EXIT_SUCCESS);
}

ex_fn_rv(execute_mvi) {
    REG(reg) = value;
}

ex_fn_val(execute_out) {
    printf("%xH (Decimal: %d)\n", REG_A, REG_A);
}

ex_fn_reg(execute_sub) {
    REG_A -= REG(reg);
}