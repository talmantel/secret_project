//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_SYMBOLS_H
#define MMN14_SYMBOLS_H
#include "definitions.h"

typedef enum {CODE, DATA, EXTERNAL} SYMBOL_TYPE;

typedef struct{
    char *name;
    unsigned int address:21;
    SYMBOL_TYPE type;
} symbol_t;


int compareSymbol(char *find, symbol_t *content);

void freeSymbolContent(symbol_t *content);


#endif //MMN14_SYMBOLS_H
