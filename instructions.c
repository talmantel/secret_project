#include <stdlib.h>
#include "instructions.h"

void freeWordContent(word_t *content){
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

void freeInstructionContent(instruction_t *instruction){
    free(instruction);
}
void freeLabelContent(label_t *label){
    free(label->label);
    free(label);
}
void freeAddressContent(address_t *address){
    free(address);
}