#include <stdlib.h>
#include "instructions.h"

void freeWordContent(void * con){
    word_t * content = (word_t *)con;
    switch (content->type) {
        case WORD_TYPE_INSTRUCTION:
            free(content->content.instruction);
            break;
        case WORD_TYPE_ADDRESS:
            free(content->content.address);
            break;
        case WORD_TYPE_LABEL:
            free(content->content.label->label);
            free(content->content.label);
            break;
    }
    free(content);
}