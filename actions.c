#include "actions.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>


RESULT checkOperands(instruction_t * instruction, const char * origOper, const char * destOper);
RESULT setAddressing(instruction_t * instruction, const char * origOper, const char * destOper);

RESULT setCommandParameters(char * command, char * origOper, char * destOper, instruction_t * instruction){
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
        if (origOper != NULL || destOper != NULL){ /* error - shouldnt have any operands*/
            return ERROR;
        } else {
            instruction->dest_addressing = instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->dest_reg = instruction->origin_reg = 0;
        }
    } else { /* error - no matching action*/
        return ERROR;
    }

    return checkOperands(instruction, origOper, destOper);
}

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
            if (origOper != NULL || destOper != NULL){ /* error - shouldnt have any operands*/
                return ERROR;
            } else {
                instruction->dest_addressing = instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
                instruction->dest_reg = instruction->origin_reg = 0;
            }
            break;
    }
    return setAddressing(instruction, origOper, destOper);
}

/*TODO: maybe return the continue action instead of result (0- error, 1- add origOper as the next inst......*/
RESULT setAddressing(instruction_t * instruction, const char * origOper, const char * destOper){
    switch (instruction->opcode){ /* check and set the addressing type..*/
        case 1: /* orig - 0,1,3 | dest - 0,1,3*/
            if (*origOper == '&'){
              return ERROR; /*unsupported addresing type*/
            } else if (*origOper == '#'){
                instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
                instruction->origin_reg = 0;
                instruction->are_type = A; /*TODO: add the number to another inst*/
            } else if (*origOper == 'r'){
                if (strlen(origOper) > 2 || !isdigit(*(origOper+1))){
                    return ERROR;
                }
                int regNum = atoi((origOper + 1));
                if (regNum < 0 || regNum > 7){
                    instruction->origin_addressing = ADDRESSING_TYPE_REGISTER;
                    instruction->origin_reg = regNum;
                }
            } else {
                instruction->origin_addressing = ADDRESSING_TYPE_DIRECT;
            }
            /*TODO: add destOper check...
            break;
        case 0: /* orig - 0,1,3 | dest - 1,3*/
        case 2:
            break;
        case 4: /* orig - 1 | dest - 1,3*/
            break;
        case 9: /* orig - NONE | dest - 1,2*/
            break;
        case 5: /* orig - NONE | dest - 1,3*/
        case 12:
            break;
        case 13: /* orig - NONE | dest - 0,1,3*/
            break;
        case 14: /* orig - NONE | dest - NONE*/
        case 15:
            instruction->dest_addressing = instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
            instruction->dest_reg = instruction->origin_reg = 0;
            break;
    }
}
