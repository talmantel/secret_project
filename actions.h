#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "instructions.h"
#include "definitions.h"

/*number of actions exist and defined*/
#define NUM_OF_ACTIONS 16

/*struct that defines an object that represent what addressing type is allowed by bitfield*/
typedef struct {
    unsigned int immediate:1;
    unsigned int direct:1;
    unsigned int relative:1;
    unsigned int reg:1;
} allowedAdd_t;

/*struct that defines action object*/
typedef struct {
    char *name;
    int opcode;
    int funct;
    union {
        allowedAdd_t addressing;
        int hasParam;/*use for easy access to check if configured*/
    } origParam;

    union {
        allowedAdd_t addressing;
        int hasParam; /*use for easy access to check if configured*/
    } destParam;
} action_t;

/*enum to hold the type of operands (origin / destination)*/
typedef enum {ORIGIN, DESTINATION} OPERAND_SLOT;

RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction);

#endif //MMN14_ACTIONS_H
