#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "instructions.h"
#include "definitions.h"

typedef struct allowedAdd {
    unsigned char immediate:1;
    unsigned char direct:1;
    unsigned char relative:1;
    unsigned char reg:1;
} allowedAdd_t;

RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction);

#endif //MMN14_ACTIONS_H
