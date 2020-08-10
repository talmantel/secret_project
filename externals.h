#ifndef MMN14_EXTERNALS_H
#define MMN14_EXTERNALS_H

typedef struct{
    char *name;
    unsigned long address;
} external_t;

void freeExternalContent(external_t *content);

#endif
