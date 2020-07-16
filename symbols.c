#include <stdlib.h>
#include <string.h>
#include "symbols.h"



int compareSymbol(void * find, void * content){
    if(strcmp((char *)find, ((symbol_t *)content)->name) == 0)
        return 1;
    return 0;
}

void freeSymbolContent(void *content){
    free(((symbol_t *)content)->name);
    free((symbol_t *)content);
}