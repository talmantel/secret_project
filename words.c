#include <stdlib.h>
#include "words.h"

/* free all the memory allocated to a word_t */
void freeWordContent(void *con){
    word_t * content = (word_t *)con;
    switch (content->type) {
        case WORD_TYPE_INSTRUCTION:
            freeInstructionContent(content->content.instruction);
            break;
        case WORD_TYPE_ADDRESS:
            freeAddressContent(content->content.address);
            break;
        case WORD_TYPE_LABEL:
            freeLabelContent(content->content.label);
            break;
    }
    free(content);
}

/* free all the memory allocated to a instruction_t */
void freeInstructionContent(instruction_t *instruction){
    free(instruction);
}

/* free all the memory allocated to a label_t */
void freeLabelContent(label_t *label){
    free(label->labelName);
    free(label);
}

/* free all the memory allocated to a address_t */
void freeAddressContent(address_t *address){
    free(address);
}
