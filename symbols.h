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
} *SYMBOL;


int compareSymbol(char *find, SYMBOL content);


#endif //MMN14_SYMBOLS_H
