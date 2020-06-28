//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_INSTRUCTIONS_H
#define MMN14_INSTRUCTIONS_H

typedef enum {E = 1, R = 2, A = 4} ARE_TYPE;
typedef enum {WORD_TYPE_INSTRUCTION, WORD_TYPE_ADDRESS, WORD_TYPE_LABEL} WORD_TYPE;
typedef enum {ADDRESSING_TYPE_IMMEDIATE = 0, ADDRESSING_TYPE_DIRECT = 1, ADDRESSING_TYPE_RELATIVE = 2, ADDRESSING_TYPE_REGISTER = 3} ADDRESSING_TYPE;

typedef struct{
    unsigned int are_type: 3;
    unsigned int funct: 5;
    unsigned int dest_reg: 3;
    ADDRESSING_TYPE dest_addressing: 2;
    unsigned int origin_reg: 3;
    ADDRESSING_TYPE origin_addressing: 2;
    unsigned int opcode: 6;
} *INSTRUCTION;

typedef struct{
    unsigned int are_type: 3;
    unsigned int address: 21;
} *ADDRESS;

typedef struct{
    WORD_TYPE type;
    union{
        INSTRUCTION instruction;
        ADDRESS address;
        char *label;
    } content;
} *WORD;

#endif //MMN14_INSTRUCTIONS_H
