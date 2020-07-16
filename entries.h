//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_ENTRIES_H
#define MMN14_ENTRIES_H

typedef struct{
    char *name;
    unsigned int address:21;
} entry_t;

void freeEntryContent(void *content);
int compareEntry(void *find, void *content);

#endif //MMN14_ENTRIES_H
