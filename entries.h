//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_ENTRIES_H
#define MMN14_ENTRIES_H

typedef struct{
    char *name;
    unsigned long address:21;
    long lineNumber;
} entry_t;

void freeEntryContent(entry_t *content);

#endif //MMN14_ENTRIES_H
