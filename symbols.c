#include <stdlib.h>
#include <string.h>
#include "symbols.h"

/*a compare function of symbol_t objects for searching in the linked list that holds symbol_t records
 * returns 1 if found a match and 0 otherwise
 * param find - a char * (string) represent the name of the symbol_t record that need to be found
 * param content - an symbol_t object to compare to*/
int compareSymbol(void * find, void * content){
    if(strcmp((char *)find, ((symbol_t *)content)->name) == 0)
        return 1;
    return 0;
}

/*function to free the symbol_t's content that was allocated
 * param content - pointer to symbol_t object that should be free.*/
void freeSymbolContent(void *content){
    free(((symbol_t *)content)->name);
    free((symbol_t *)content);
}