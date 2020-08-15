#ifndef MMN14_WORDS_H
#define MMN14_WORDS_H

typedef enum {E = 1, R = 2, A = 4} ARE_TYPE;
typedef enum {WORD_TYPE_INSTRUCTION, WORD_TYPE_ADDRESS, WORD_TYPE_LABEL} WORD_TYPE;
typedef enum {ADDRESSING_TYPE_IMMEDIATE = 0, ADDRESSING_TYPE_DIRECT = 1, ADDRESSING_TYPE_RELATIVE = 2, ADDRESSING_TYPE_REGISTER = 3} ADDRESSING_TYPE;

typedef struct{
    unsigned int are_type: 3;
    unsigned int funct: 5;
    unsigned int dest_reg: 3;
    ADDRESSING_TYPE dest_addressing;
    unsigned int origin_reg: 3;
    ADDRESSING_TYPE origin_addressing;
    unsigned int opcode: 6;
} instruction_t;

typedef struct{
    unsigned int are_type: 3;
    unsigned long addressValue;
} address_t;

typedef struct {
    char *labelName;
    long lineNumber;
    ADDRESSING_TYPE addressing_type;
} label_t;

typedef struct{
    WORD_TYPE type;
    union{
        instruction_t *instruction;
        address_t *address;
        label_t *label ;
    } content;
} word_t;

/* free all the memory allocated to a word_t */
void freeWordContent(void *con);

/* free all the memory allocated to a instruction_t */
void freeInstructionContent(instruction_t *content);

/* free all the memory allocated to a label_t */
void freeLabelContent(label_t *content);

/* free all the memory allocated to a address_t */
void freeAddressContent(address_t *content);

#endif
