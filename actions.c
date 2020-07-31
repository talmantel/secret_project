#include "actions.h"
#include "errors.h"
#include "labels.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

RESULT checkOperands(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper, action_t *action);

RESULT checkOperand(const char *fileName, long lineNum, instruction_t *instruction, const char *operand,
                    allowedAdd_t allowedAddressing, OPERAND_SLOT operandSlot);

action_t actions[NUM_OF_ACTIONS] = {
        "mov", 0, 0, {.addressing = {1,1,0,1}}, {.addressing = {0, 1, 0, 1}},
        "cmp", 1, 0, {.addressing = {1,1,0,1}}, {.addressing = {1, 1, 0, 1}},
        "add", 2, 1, {.addressing = {1,1,0,1}}, {.addressing = {0, 1, 0, 1}},
        "sub", 2, 2, {.addressing = {1,1,0,1}}, {.addressing = {0, 1, 0, 1}},
        "lea", 4, 0, {.addressing = {0,1,0,0}}, {.addressing = {0, 1, 0, 1}},
        "clr", 5, 1, {.hasParam = 0}, {.addressing = {0, 1, 0, 1}},
        "not", 5, 2, {.hasParam = 0}, {.addressing = {0, 1, 0, 1}},
        "inc", 5, 3, {.hasParam = 0}, {.addressing = {0, 1, 0, 1}},
        "dec", 5, 4, {.hasParam = 0}, {.addressing = {0, 1, 0, 1}},
        "jmp", 9, 1, {.hasParam = 0}, {.addressing = {0, 1, 1, 0}},
        "bne", 9, 2, {.hasParam = 0}, {.addressing = {0, 1, 1, 0}},
        "jsr", 9, 3, {.hasParam = 0}, {.addressing = {0, 1, 1, 0}},
        "red", 12, 0, {.hasParam = 0}, {.addressing = {0, 1, 0, 1}},
        "prn", 13, 0, {.hasParam = 0}, {.addressing = {1, 1, 0, 1}},
        "rts", 14, 0, {.hasParam = 0}, {.hasParam = 0},
        "stop", 15, 0, {.hasParam = 0}, {.hasParam = 0}
};

/*checking the command inserted and setting the opcode and funct
 * if no matching command has been found returning ERROR.
 * otherwise continue checking the operands inserted */
RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction) {
    instruction->are_type = A;
    int i;
    for(i = 0; i < NUM_OF_ACTIONS; i++){
        if(strcmp(actions[i].name, command) == 0){
            instruction->funct = actions[i].funct;
            instruction->opcode = actions[i].opcode;
            return checkOperands(fileName, lineNum, instruction, origOper, destOper, &actions[i]);
        }
    }

   /* error - no matching action*/
    printError(fileName, lineNum, "command '%s' does not exist!\n", command);
    return ERROR;
}

/*checking if the operands inserted exist (not empty).
 * if the operand type isn't matching the command - returning ERROR.
 * otherwise continue checking addressing types for the operands and setting instruction' properties */
RESULT checkOperands(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper, action_t *action) {

    RESULT result = SUCCESS, tempResult;

    if ((action->origParam.hasParam && origOper == NULL) || (action->destParam.hasParam && destOper == NULL)) {
        if (action->origParam.hasParam)
            printError(fileName, lineNum, "missing operand\\s. Command requires two operands!\n", NULL);
        else
            printError(fileName, lineNum, "missing operand\\s. Command requires one operand!\n", NULL);
        return ERROR;
    }

    if ((!action->origParam.hasParam && origOper != NULL) || (!action->destParam.hasParam && destOper != NULL)) {
        if (action->origParam.hasParam)
            printError(fileName, lineNum, "too many operands. Command requires no operands!\n", NULL);
        else
            printError(fileName, lineNum, "too many operands. Command requires one operand!\n", NULL);
        return ERROR;
    }

    /* initialize values */
    instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    instruction->origin_reg = 0;
    instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
    instruction->dest_reg = 0;

    if (origOper != NULL) {
        tempResult = checkOperand(fileName, lineNum, instruction, origOper, action->origParam.addressing, ORIGIN);
        if (tempResult == ERROR)
            return ERROR;
        result += tempResult;
    }

    if (destOper != NULL) {
        tempResult = checkOperand(fileName, lineNum, instruction, destOper, action->destParam.addressing, DESTINATION);
        if (tempResult == ERROR)
            return ERROR;
        result += tempResult;
    }

    return result;
}

/* checking operand and comparing it to the addressing type allowed for this command.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be appended).*/
RESULT checkOperand(const char *fileName, long lineNum, instruction_t *instruction, const char *operand,
                    allowedAdd_t allowedAddressing, OPERAND_SLOT operandSlot) {
    ADDRESSING_TYPE addressingType;
    int i, reg = 0, shouldAppend = 0;

    if (operand[0] == '&'){ /*addressing type 2*/
        if(allowedAddressing.relative){
            if(checkIfLegalLabel(fileName, lineNum, operand+1) == ERROR) {
                /* error printed in checkIfLegalLabel */
                return ERROR;
            }

            addressingType = ADDRESSING_TYPE_RELATIVE;
            shouldAppend = 1;
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    } else if (operand[0] == '#'){ /*addressing type 0*/
        if (allowedAddressing.immediate) {
            for(i = 1; operand[i] != '\0'; i++){ /*checking if contains only numbers with leading minus or plus sign*/
                if(!isdigit(operand[i])){ /* none number parameter in operand*/
                    if ((operand[i] == '-' || operand[i] == '+') && i != 1) { /* has minus or plus but not in the lead*/
                        printError(fileName, lineNum, "operand '%s' is not a number!\n", operand);
                        return ERROR;
                    }
                }
            }
            addressingType = ADDRESSING_TYPE_IMMEDIATE;
            shouldAppend = 1;
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    } else if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' &&  operand[1] <= '7'){ /*addressing type 3*/
        if(allowedAddressing.reg) {
            addressingType = ADDRESSING_TYPE_REGISTER;
            reg = atoi(operand + 1); /* get numeric value */
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    } else { /*addressing type 1*/
        if (allowedAddressing.direct) {
            if(checkIfLegalLabel(fileName, lineNum, operand) == ERROR) {
                /* error printed in checkIfLegalLabel */
                return ERROR;
            }

            addressingType = ADDRESSING_TYPE_DIRECT;
            shouldAppend = 1;
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    }

    if(operandSlot == ORIGIN) {
        instruction->origin_addressing = addressingType;
        instruction->origin_reg = reg;
        if (shouldAppend)
            return APPEND_FOR_ORIG;
    } else if(operandSlot == DESTINATION) {
        instruction->dest_addressing = addressingType;
        instruction->dest_reg = reg;
        if (shouldAppend)
            return APPEND_FOR_DEST;
    }

    return SUCCESS;
}