#ifndef MMN14_EXTERN_H
#define MMN14_EXTERN_H

/*struct that represent an extern record*/
typedef struct{
    char *name;
    unsigned long address:21;
} extern_t;

void freeExternContent(extern_t *content);

#endif //MMN14_EXTERN_H
