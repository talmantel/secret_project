#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "instructions.h"
#include "definitions.h"

#define NUM_OF_ACTIONS 16

typedef struct {
    unsigned int immediate:1;
    unsigned int direct:1;
    unsigned int relative:1;
    unsigned int reg:1;
} allowedAdd_t;

typedef struct {
    char *name;
    int opcode;
    int funct;
    union {
        allowedAdd_t addressing;
        int hasParam;
    } origParam;

    union {
        allowedAdd_t addressing;
        int hasParam;
    } destParam;
} action_t;

typedef enum {ORIGIN, DESTINATION} OPERAND_SLOT;

RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction);

#endif
