#include <stdlib.h>
#include <string.h>
#include "entries.h"

/*a compare function of entry_t objects for searching in the linked list that holds entry_t records
 * returns 1 if found a match and 0 otherwise
 * param find - a char * (string) represents the name of the entry_t record that needs to be found
 * param content - an entry_t object to compare to*/
int compareEntry(void * find, void * content){
    if(strcmp((char *)find, ((entry_t *)content)->name) == 0)
        return 1;
    return 0;
}

/*function to free the entry_t's content that was allocated
 * param content - pointer to entry_t object that should be freed.*/
void freeEntryContent(void *content){
    free(((entry_t *)content)->name);
    free((entry_t *)content);
}