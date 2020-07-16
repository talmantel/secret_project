#include <stdlib.h>
#include <string.h>
#include "entries.h"

int compareEntry(void * find, void * content){
    if(strcmp((char *)find, ((entry_t *)content)->name) == 0)
        return 1;
    return 0;
}

void freeEntryContent(void *content){
    free(((entry_t *)content)->name);
    free((entry_t *)content);
}