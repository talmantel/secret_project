#ifndef MMN14_EXTERNALS_H
#define MMN14_EXTERNALS_H

typedef struct{
    char *name;
    unsigned long address:21;
} external_t;

void freeExternalContent(external_t *content);

#endif //MMN14_EXTERNALS_H
