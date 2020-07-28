#include "actions.h"
#include "errors.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {ORIGIN, DESTINATION} OPERAND_SLOT;

RESULT checkOperands(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper);
RESULT setAddressing(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper);
RESULT checkOperand(const char *fileName, long lineNum, instruction_t *instruction, const char *operand,
                    allowedAdd_t allowedAddressing, OPERAND_SLOT operandSlot);

/*checking the command inserted and setting the opcode and funct
 * if no matching command has been found returning ERROR.
 * otherwise continue checking the operands inserted */
RESULT setCommandParameters(const char *fileName, long lineNum, char *command, char *origOper, char *destOper,
                            instruction_t *instruction) {
    instruction->are_type = A;
    if (strcmp(command, "mov") == 0){
        instruction->funct = 0;
        instruction->opcode = 0;
    } else if (strcmp(command, "cmp") == 0){
        instruction->funct = 0;
        instruction->opcode = 1;
    } else if (strcmp(command, "add") == 0){
        instruction->funct = 1;
        instruction->opcode = 2;
    } else if (strcmp(command, "sub") == 0){
        instruction->funct = 2;
        instruction->opcode = 2;
    } else if (strcmp(command, "lea") == 0){
        instruction->funct = 0;
        instruction->opcode = 4;
    } else if (strcmp(command, "clr") == 0){
        instruction->funct = 1;
        instruction->opcode = 5;
    } else if (strcmp(command, "not") == 0){
        instruction->funct = 2;
        instruction->opcode = 5;
    } else if (strcmp(command, "inc") == 0){
        instruction->funct = 3;
        instruction->opcode = 5;
    } else if (strcmp(command, "dec") == 0){
        instruction->funct = 4;
        instruction->opcode = 5;
    } else if (strcmp(command, "jmp") == 0){
        instruction->funct = 1;
        instruction->opcode = 9;
    } else if (strcmp(command, "bne") == 0){
        instruction->funct = 2;
        instruction->opcode = 9;
    } else if (strcmp(command, "jsr") == 0){
        instruction->funct = 3;
        instruction->opcode = 9;
    } else if (strcmp(command, "red") == 0){
        instruction->funct = 0;
        instruction->opcode = 12;
    } else if (strcmp(command, "prn") == 0){
        instruction->funct = 0;
        instruction->opcode = 13;
    } else if (strcmp(command, "rts") == 0){
        instruction->funct = 0;
        instruction->opcode = 14;
    } else if (strcmp(command, "stop") == 0){
        instruction->funct = 0;
        instruction->opcode = 15;
    } else { /* error - no matching action*/
        printError(fileName, lineNum, "command '%s' does not exist!\n", command);
        return ERROR;
    }

    return checkOperands(fileName, lineNum, instruction, origOper, destOper);
}

/*checking if the operands inserted exist (not empty).
 * if the operand type isn't matching the command - returning ERROR.
 * otherwise continue checking addressing types for the operands and setting instruction' properties */
RESULT checkOperands(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper) {
    switch ((int)instruction->opcode){ /* check num of operands..*/
        case 0:
        case 1:
        case 2:
        case 4:
            if (origOper == NULL || destOper == NULL){ /*error - must have 2 operands*/
                printError(fileName, lineNum, "missing operand\\s. Command requires two operads!\n", NULL);
                return ERROR;
            }
            break;
        case 5:
        case 9:
        case 12:
        case 13:
            if (origOper != NULL){ /* error - should not have orig operand*/
                printError(fileName, lineNum, "too many operands. Command requires one operand!\n", NULL);
                return ERROR;
            }
            if (destOper == NULL){ /* error - should have dest operand*/
                printError(fileName, lineNum, "missing operand. Command requires one operand!\n", NULL);
                return ERROR;
            }
            instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->origin_reg = 0;
            break;
        case 14:
        case 15:
            if (origOper != NULL || destOper != NULL){ /* error - shouldn't have any operands*/
                printError(fileName, lineNum, "too many operands. Command requires no operands!\n", NULL);
                return ERROR;
            }
            instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->origin_reg = 0;
            instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->dest_reg = 0;
            break;
    }
    return setAddressing(fileName, lineNum, instruction, origOper, destOper);
}

/* checking each operand and comparing it to the addressing type allowed for this command.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be append).*/
RESULT setAddressing(const char *fileName, long lineNum, instruction_t *instruction, const char *origOper,
                     const char *destOper) {
    RESULT result = 0, tempResult;
    allowedAdd_t origAllowedAdd, destAllowedAdd;
    switch ((int)instruction->opcode){ /* check and set the addressing type..*/
        case 1: /* orig - 0,1,3 | dest - 0,1,3*/
            origAllowedAdd.immediate = destAllowedAdd.immediate = 1;
            origAllowedAdd.direct = destAllowedAdd.direct = 1;
            origAllowedAdd.relative = destAllowedAdd.relative = 0;
            origAllowedAdd.reg = destAllowedAdd.reg = 1;
            if((tempResult = checkOperand(fileName, lineNum, instruction, origOper, origAllowedAdd, ORIGIN)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 0: /* orig - 0,1,3 | dest - 1,3*/
        case 2:
            origAllowedAdd.immediate = 1;
            destAllowedAdd.immediate = 0;
            origAllowedAdd.direct = destAllowedAdd.direct = 1;
            origAllowedAdd.relative = destAllowedAdd.relative = 0;
            origAllowedAdd.reg = destAllowedAdd.reg = 1;
            if((tempResult = checkOperand(fileName, lineNum, instruction, origOper, origAllowedAdd, ORIGIN)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 4: /* orig - 1 | dest - 1,3*/
            origAllowedAdd.immediate = destAllowedAdd.immediate = 0;
            origAllowedAdd.direct = destAllowedAdd.direct = 1;
            origAllowedAdd.relative = destAllowedAdd.relative = 0;
            origAllowedAdd.reg = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkOperand(fileName, lineNum, instruction, origOper, origAllowedAdd, ORIGIN)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 9: /* orig - NONE | dest - 1,2*/
            destAllowedAdd.immediate = 0;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 1;
            destAllowedAdd.reg = 0;
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 5: /* orig - NONE | dest - 1,3*/
        case 12:
            destAllowedAdd.immediate = 0;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 13: /* orig - NONE | dest - 0,1,3*/
            destAllowedAdd.immediate = 1;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkOperand(fileName, lineNum, instruction, destOper, destAllowedAdd, DESTINATION)) != ERROR){
                result += tempResult;
            } else {
                /* error was printed in checkOperand*/
                return ERROR;
            }
            break;
        case 14: /* orig - NONE | dest - NONE*/
        case 15:  /*no need to checking anything. checks already made for this type */
            instruction->dest_addressing = instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->dest_reg = instruction->origin_reg = 0;
            break;
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
            addressingType = ADDRESSING_TYPE_RELATIVE;
            shouldAppend = 1;
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand!\n", operand);
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
            printError(fileName, lineNum, "wrong addressing type for operand!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    } else if (operand[0] == 'r' && strlen(operand) == 2 && operand[1] >= '0' &&  operand[1] <= '7'){ /*addressing type 3*/
        if(allowedAddressing.reg) {
            addressingType = ADDRESSING_TYPE_REGISTER;
            reg = atoi(operand + 1); /* get numeric value */
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand!\n", operand);
            return ERROR; /*unsupported addressing type*/
        }
    } else { /*addressing type 1*/
        if (allowedAddressing.direct) {
            addressingType = ADDRESSING_TYPE_DIRECT;
            shouldAppend = 1;
        } else {
            printError(fileName, lineNum, "wrong addressing type for operand!\n", operand);
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