#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "instructions.h"
#include "definitions.h"

typedef struct action{
    unsigned int funct : 5;
    unsigned int opcode : 5;
} action_t;

RESULT setCommandParameters(char * command, char * origOper, char * destOper, instruction_t * instruction);

#endif //MMN14_ACTIONS_H
