#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "instructions.h"
#include "definitions.h"

typedef struct allowedAdd {
    char immediate:1;
    char direct:1;
    char relative:1;
    char reg:1;
} allowedAdd_t;

RESULT setCommandParameters(char * command, char * origOper, char * destOper, instruction_t * instruction);

#endif //MMN14_ACTIONS_H
