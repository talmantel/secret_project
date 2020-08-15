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

RESULT checkAddressingTypeZero(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing);
RESULT checkAddressingTypeOne(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing);
RESULT checkAddressingTypeTwo(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing);
RESULT checkAddressingTypeThree(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing,
                              int *reg);

/*Array to hold the actions list and their parameters (command, opcode, funct, and available addressing type of operands if exist*/
action_t actions[NUM_OF_ACTIONS] = {
        {"mov",   0, 0, {{1, 1, 0, 1}}, {{0, 1, 0, 1}}},
        {"cmp",   1, 0, {{1, 1, 0, 1}}, {{1, 1, 0, 1}}},
        {"add",   2, 1, {{1, 1, 0, 1}}, {{0, 1, 0, 1}}},
        {"sub",   2, 2, {{1, 1, 0, 1}}, {{0, 1, 0, 1}}},
        {"lea",   4, 0, {{0, 1, 0, 0}}, {{0, 1, 0, 1}}},
        {"clr",   5, 1, {{0, 0, 0, 0}}, {{0, 1, 0, 1}}},
        {"not",   5, 2, {{0, 0, 0, 0}}, {{0, 1, 0, 1}}},
        {"inc",   5, 3, {{0, 0, 0, 0}}, {{0, 1, 0, 1}}},
        {"dec",   5, 4, {{0, 0, 0, 0}}, {{0, 1, 0, 1}}},
        {"jmp",   9, 1, {{0, 0, 0, 0}}, {{0, 1, 1, 0}}},
        {"bne",   9, 2, {{0, 0, 0, 0}}, {{0, 1, 1, 0}}},
        {"jsr",   9, 3, {{0, 0, 0, 0}}, {{0, 1, 1, 0}}},
        {"red",  12, 0, {{0, 0, 0, 0}}, {{0, 1, 0, 1}}},
        {"prn",  13, 0, {{0, 0, 0, 0}}, {{1, 1, 0, 1}}},
        {"rts",  14, 0, {{0, 0, 0, 0}}, {{0, 0, 0, 0}}},
        {"stop", 15, 0, {{0, 0, 0, 0}}, {{0, 0, 0, 0}}}
};

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
                            instruction_t *instruction) {
    int i;
    instruction->are_type = A; /* all instruction's word are with A flag on.*/

    /*loop through the commands array and check if the inserted command exists*/
    for(i = 0; i < NUM_OF_ACTIONS; i++){
        if(strcmp(actions[i].name, command) == 0){
            instruction->funct = actions[i].funct;
            instruction->opcode = actions[i].opcode;
            return checkOperands(fileName, lineNum, instruction, origOper, destOper, &actions[i]);
        }
    }

   /* error - no matching action*/
    printErrorWithLine(fileName, lineNum, "command '%s' does not exist!\n", command);
    return ERROR;
}

/*checking if the operands inserted exist (not empty).
 * if the operand type isn't matching the command - returning ERROR.
 * otherwise continue checking addressing types for the operands and setting instruction's parameters and returning
 * what should be done next - adding word for origin\destination operand or not.
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param instruction - a pointer to an instruction object to assign parameters to
 * param origOper - the origin operand if exist
 * param destOper - the destination operand if exist
 * param action - the action with his parameters */
RESULT checkOperands(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper, action_t *action) {

    RESULT result = SUCCESS, tempResult;

    if ((action->origParam.hasParam && origOper == NULL) || (action->destParam.hasParam && destOper == NULL)) {
        if (action->origParam.hasParam)
            printErrorWithLine(fileName, lineNum, "missing operand\\s. Command requires two operands!\n", NULL);
        else
            printErrorWithLine(fileName, lineNum, "missing operand\\s. Command requires one operand!\n", NULL);
        return ERROR;
    }

    if ((!action->origParam.hasParam && origOper != NULL) || (!action->destParam.hasParam && destOper != NULL)) {
        if (action->destParam.hasParam)
            printErrorWithLine(fileName, lineNum, "too many operands. Command requires one operand!\n", NULL);
        else
            printErrorWithLine(fileName, lineNum, "too many operands. Command requires no operands!\n", NULL);
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

/* checking operand and comparing it to the addressing type allowed for this action.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be appended).
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param instruction - a pointer to an instruction object to assign parameters to
 * param operand - operand's string to be checked
 * param allowedAddressing - action's allowed addressing types
 * param operandSlot - use as a flag to determine if its origin or destination operand*/
RESULT checkOperand(const char *fileName, long lineNum, instruction_t *instruction, const char *operand,
                    allowedAdd_t allowedAddressing, OPERAND_SLOT operandSlot) {
    ADDRESSING_TYPE addressingType;
    int reg = 0, shouldAppend = 1;

    if (operand[0] == '&'){ /*addressing type 2*/
        if (checkAddressingTypeTwo(fileName, lineNum, operand, allowedAddressing) == ERROR){
            return ERROR;
        }
        addressingType = ADDRESSING_TYPE_RELATIVE;
    } else if (operand[0] == '#'){ /*addressing type 0*/
        if (checkAddressingTypeZero(fileName, lineNum, operand, allowedAddressing) == ERROR){
            return ERROR;
        }
        addressingType = ADDRESSING_TYPE_IMMEDIATE;
    } else if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' &&  operand[1] <= '7'){ /*addressing type 3*/
        if (checkAddressingTypeThree(fileName, lineNum, operand, allowedAddressing, &reg) == ERROR){
            return ERROR;
        }
        addressingType = ADDRESSING_TYPE_REGISTER;
        shouldAppend = 0;
    } else { /*addressing type 1*/
        if (checkAddressingTypeOne(fileName, lineNum, operand, allowedAddressing) == ERROR){
            return ERROR;
        }
        addressingType = ADDRESSING_TYPE_DIRECT;
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

/* checking addressing type zero (immediate addressing)  and making sure the operand follow the addressing type's rules.
 * returns ERROR if the operand don't follow the rules. otherwise returns SUCCESS.
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param operand - operand's string to be checked
 * param allowedAddressing - action's allowed addressing types */
RESULT checkAddressingTypeZero(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing){
    int i;
    if (allowedAddressing.immediate) {
        for(i = 1; operand[i] != '\0'; i++){ /*checking if contains only numbers with leading minus or plus sign*/
            if(!isdigit(operand[i])){ /* none number parameter in operand*/
                if ((operand[i] == '-' || operand[i] == '+') && i != 1) { /* has minus or plus but not in the lead*/
                    printErrorWithLine(fileName, lineNum, "operand '%s' is not a number!\n", operand);
                    return ERROR;
                }
            }
        }
    } else {
        printErrorWithLine(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
        return ERROR; /*unsupported addressing type*/
    }
    return SUCCESS;
}

/* checking addressing type one (direct addressing) and making sure the operand follow the addressing type's rules.
 * returns ERROR if the operand don't follow the rules. otherwise returns SUCCESS.
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param operand - operand's string to be checked
 * param allowedAddressing - action's allowed addressing types */
RESULT checkAddressingTypeOne(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing){
    if (allowedAddressing.direct) {
        if(checkIfLegalLabel(fileName, lineNum, operand) == ERROR) {
            /* error printed in checkIfLegalLabel */
            return ERROR;
        }
    } else {
        printErrorWithLine(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
        return ERROR; /*unsupported addressing type*/
    }
    return SUCCESS;
}

/* checking addressing type two (relative addressing) and making sure the operand follow the addressing type's rules.
 * returns ERROR if the operand don't follow the rules. otherwise returns SUCCESS.
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param operand - operand's string to be checked
 * param allowedAddressing - action's allowed addressing types */
RESULT checkAddressingTypeTwo(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing){
    if(allowedAddressing.relative){
        if(checkIfLegalLabel(fileName, lineNum, operand+1) == ERROR) {
            /* error printed in checkIfLegalLabel */
            return ERROR;
        }
    } else {
        printErrorWithLine(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
        return ERROR; /*unsupported addressing type*/
    }
    return SUCCESS;
}

/* checking addressing type three (register addressing) and making sure the operand follow the addressing type's rules.
 * returns ERROR if the operand don't follow the rules. otherwise returns SUCCESS.
 * param fileName - the .as file we working on
 * param lineNum - the line number we are currently parsing
 * param operand - operand's string to be checked
 * param allowedAddressing - action's allowed addressing types
 * param reg - a pointer to int to hold the reg number parsed*/
RESULT checkAddressingTypeThree(const char *fileName, long lineNum, const char *operand, allowedAdd_t allowedAddressing,
                              int *reg){
    if(allowedAddressing.reg) {
        *reg = atoi(operand + 1); /* get numeric value */
    } else {
        printErrorWithLine(fileName, lineNum, "wrong addressing type for operand '%s'!\n", operand);
        return ERROR; /*unsupported addressing type*/
    }
    return SUCCESS;
}