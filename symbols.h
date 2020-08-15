#ifndef MMN14_SYMBOLS_H
#define MMN14_SYMBOLS_H
#include "definitions.h"

typedef enum {CODE, DATA, EXTERNAL} SYMBOL_TYPE;

/* a struct to represent a symbol*/
typedef struct{
    char *name;
    unsigned long address;
    SYMBOL_TYPE type;
} symbol_t;


/*a compare function of symbol_t objects for searching in the linked list that holds symbol_t records
 * returns 1 if found a match and 0 otherwise
 * param find - a char * (string) represent the name of the symbol_t record that need to be found
 * param content - an symbol_t object to compare to*/
int compareSymbol(void * find, void * content);

/*function to free the symbol_t's content that was allocated
 * param content - pointer to symbol_t object that should be freed.*/
void freeSymbolContent(void *content);


#endif
