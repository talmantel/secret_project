#include "actions.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

typedef struct allowedAdd {
    char immediate:1;
    char direct:1;
    char relative:1;
    char reg:1;
}allowedAdd_t;

RESULT checkOperands(instruction_t * instruction, const char * origOper, const char * destOper);
RESULT setAddressing(instruction_t * instruction, const char * origOper, const char * destOper);
RESULT checkOriginOperand(instruction_t * instruction, const char * operand, allowedAdd_t allowedAddressing);
RESULT checkDestOperand(instruction_t * instruction, const char * operand, allowedAdd_t allowedAddressing);

/*checking the command inserted and setting the opcode and funct
 * if no matching command has been found returning ERROR.
 * otherwise continue checking the operands inserted */
RESULT setCommandParameters(char * command, char * origOper, char * destOper, instruction_t * instruction){
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
    } else if (strcmp(command, "rt") == 0){
        instruction->funct = 0;
        instruction->opcode = 14;
    } else if (strcmp(command, "stop") == 0){
        instruction->funct = 0;
        instruction->opcode = 15;
    } else { /* error - no matching action*/
        return ERROR;
    }

    return checkOperands(instruction, origOper, destOper);
}

/*checking if the operands inserted exist (not empty).
 * if the operand type isn't matching the command - returning ERROR.
 * otherwise continue checking addressing types for the operands and setting instruction' properties */
RESULT checkOperands(instruction_t * instruction, const char * origOper, const char * destOper){
    switch (instruction->opcode){ /* check num of operands..*/
        case 0:
        case 1:
        case 2:
        case 4:
            if (origOper == NULL || destOper == NULL){ /*error - must have 2 operands*/
                return ERROR;
            }
            break;
        case 5:
        case 9:
        case 12:
        case 13:
            if (origOper != NULL || destOper == NULL){ /* error - most have only destination operand*/
                return ERROR;
            }
            break;
        case 14:
        case 15:
            if (origOper != NULL || destOper != NULL){ /* error - shouldn't have any operands*/
                return ERROR;
            }
            break;
    }
    return setAddressing(instruction, origOper, destOper);
}

/* checking each operand and comparing it to the addressing type allowed for this command.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be append).*/
RESULT setAddressing(instruction_t * instruction, const char * origOper, const char * destOper){
    RESULT result = 0, tempResult;
    allowedAdd_t origAllowedAdd, destAllowedAdd;
    switch (instruction->opcode){ /* check and set the addressing type..*/
        case 1: /* orig - 0,1,3 | dest - 0,1,3*/
            origAllowedAdd.immediate = destAllowedAdd.immediate = 1;
            origAllowedAdd.direct = destAllowedAdd.direct = 1;
            origAllowedAdd.relative = destAllowedAdd.relative = 0;
            origAllowedAdd.reg = destAllowedAdd.reg = 1;
            if((tempResult = checkOriginOperand(instruction, origOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
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
            if((tempResult = checkOriginOperand(instruction, origOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            break;
        case 4: /* orig - 1 | dest - 1,3*/
            origAllowedAdd.immediate = destAllowedAdd.immediate = 0;
            origAllowedAdd.direct = destAllowedAdd.direct = 1;
            origAllowedAdd.relative = destAllowedAdd.relative = 0;
            origAllowedAdd.reg = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkOriginOperand(instruction, origOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            break;
        case 9: /* orig - NONE | dest - 1,2*/
            destAllowedAdd.immediate = 0;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 1;
            destAllowedAdd.reg = 0;
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            break;
        case 5: /* orig - NONE | dest - 1,3*/
        case 12:
            destAllowedAdd.immediate = 0;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
                return ERROR;
            }
            break;
        case 13: /* orig - NONE | dest - 0,1,3*/
            destAllowedAdd.immediate = 1;
            destAllowedAdd.direct = 1;
            destAllowedAdd.relative = 0;
            destAllowedAdd.reg = 1;
            if((tempResult = checkDestOperand(instruction, destOper, origAllowedAdd)) != -1){
                result += tempResult;
            } else {
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

/* checking origin operand and comparing it to the addressing type allowed for this command.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be append).*/
RESULT checkOriginOperand(instruction_t * instruction, const char * operand, allowedAdd_t allowedAddressing){
    if (*operand == '&'){ /*addressing type 2*/
        if(allowedAddressing.relative){
            instruction->origin_addressing = ADDRESSING_TYPE_RELATIVE;
            instruction->origin_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else if (*operand == '#'){ /*addressing type 0*/
        if (allowedAddressing.immediate) {
            int i;
            for(i = 1; i < strlen(operand) - 1; i++){ /*checking if contains only numbers with leading minus or plus sign*/
                if(!isdigit(operand[i])){ /* none number parameter in operand*/
                    if ((operand[i] == '-' || operand[i] == '+') && i != 1) { /* has minus or plus but not in the lead*/
                        return ERROR;
                    }
                }
            }
            instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->origin_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else if (*operand == 'r'){ /*addressing type 3*/
        if (allowedAddressing.reg) {
            if (strlen(operand) > 2 || !isdigit(*(operand + 1))) {
                return ERROR; /*error parsing register name*/
            }
            int regNum = atoi((operand + 1));
            if (regNum < 0 || regNum > 7) {
                instruction->origin_addressing = ADDRESSING_TYPE_REGISTER;
                instruction->origin_reg = regNum;
                return SUCCESS; /* no need to append another word*/
            } else {
                return ERROR; /* reg number isn't exist*/
            }
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else { /*addressing type 1*/
        if (allowedAddressing.direct) {
            instruction->origin_addressing = ADDRESSING_TYPE_DIRECT;
            instruction->origin_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    }
    return APPEND_FOR_ORIG;
}

/* checking destination operand and comparing it to the addressing type allowed for this command.
 * if something is wrong with the operands (using unsupported addressing type/wrong input in the operand) returns ERROR.
 * otherwise returning RESULT as the next action to do (how many and what words should be append).*/
RESULT checkDestOperand(instruction_t * instruction, const char * operand, allowedAdd_t allowedAddressing){
    if (*operand == '&'){ /*addressing type 2*/
        if(allowedAddressing.relative){
            instruction->dest_addressing = ADDRESSING_TYPE_RELATIVE;
            instruction->dest_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else if (*operand == '#'){ /*addressing type 0*/
        if (allowedAddressing.immediate) {
            int i;
            for(i = 1; i < strlen(operand) - 1; i++){ /*checking if contains only numbers with leading minus or plus sign*/
                if(!isdigit(operand[i])){ /* none number parameter in .operand*/
                    if ((operand[i] == '-' || operand[i] == '+') && i != 1) { /* has minus or plus but not in the lead*/
                        return ERROR;
                    }
                }
            }
            instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->dest_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else if (*operand == 'r'){ /*addressing type 3*/
        if (allowedAddressing.reg) {
            if (strlen(operand) > 2 || !isdigit(*(operand + 1))) {
                return ERROR; /*error parsing register name*/
            }
            int regNum = atoi((operand + 1));
            if (regNum < 0 || regNum > 7) {
                instruction->dest_addressing = ADDRESSING_TYPE_REGISTER;
                instruction->dest_reg = regNum;
                return SUCCESS; /* no need to append another word*/
            } else {
                return ERROR; /* reg number isn't exist*/
            }
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    } else { /*addressing type 1*/
        if (allowedAddressing.direct) {
            instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
            instruction->dest_reg = 0;
        } else {
            return ERROR; /*unsupported addressing type*/
        }
    }
    return APPEND_FOR_DEST;
}
