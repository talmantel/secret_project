#include <stdlib.h>
#include <string.h>
#include "symbols.h"

int compareSymbol(char *find, symbol_t *content){
    if(strcmp(find, content->name) == 0)
        return 1;
    return 0;
}

void freeSymbolContent(symbol_t *content){
    free(content->name);
    free(content);
}