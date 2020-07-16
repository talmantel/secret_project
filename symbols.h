//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_SYMBOLS_H
#define MMN14_SYMBOLS_H
#include "definitions.h"

typedef enum {CODE, DATA, ENTRY, EXTERNAL} SYMBOL_TYPE;

typedef struct{
    char *name;
    unsigned long address:21;
    SYMBOL_TYPE type;
} symbol_t;


int compareSymbol(void * find, void * content);

void freeSymbolContent(void *content);


#endif //MMN14_SYMBOLS_H
