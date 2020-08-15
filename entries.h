#ifndef MMN14_ENTRIES_H
#define MMN14_ENTRIES_H

/*struct to represent an entry object and his params*/
typedef struct{
    char *name;
    unsigned long address;
    long lineNumber;
} entry_t;

/*function to free the entry_t's content that was allocated
 * param content - pointer to entry_t object that should be freed.*/
void freeEntryContent(void *content);

/*a compare function of entry_t objects for searching in the linked list that holds entry_t records
 * returns 1 if found a match and 0 otherwise
 * param find - a char * (string) represents the name of the entry_t record that needs to be found
 * param content - an entry_t object to compare to*/
int compareEntry(void *find, void *content);

#endif
