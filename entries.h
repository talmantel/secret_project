#ifndef MMN14_ENTRIES_H
#define MMN14_ENTRIES_H

/*struct to represent an entry object and his params*/
typedef struct{
    char *name;
    unsigned long address;
    long lineNumber;
} entry_t;

void freeEntryContent(void *content);
int compareEntry(void *find, void *content);

#endif
