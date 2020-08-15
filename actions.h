#ifndef MMN14_ACTIONS_H
#define MMN14_ACTIONS_H

#include "words.h"
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

/*checking the command inserted and setting the opcode and funct
 * if no matching action has been found returning ERROR.
 * otherwise continue checking the operands inserted
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param command - the command's string
 * param origOper - the origin operand if exists
 * param destOper - the destination operand if exists
 * param instruction - a pointer to an instruction object to assign parameters to*/
RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction);

#endif
