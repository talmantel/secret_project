#include <stdlib.h>
#include "entries.h"

void freeEntryContent(entry_t *content){
    free(content->name);
    free(content);
}