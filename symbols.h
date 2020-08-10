#ifndef MMN14_SYMBOLS_H
#define MMN14_SYMBOLS_H
#include "definitions.h"

typedef enum {CODE, DATA, EXTERNAL} SYMBOL_TYPE;

/* a struct to represent a symbol*/
typedef struct{
    char *name;
    unsigned long address:21;
    SYMBOL_TYPE type;
} symbol_t;


int compareSymbol(void * find, void * content);
void freeSymbolContent(void *content);

#endif //MMN14_SYMBOLS_H
