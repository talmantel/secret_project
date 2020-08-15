#ifndef MMN14_EXTERN_H
#define MMN14_EXTERN_H

/*struct that represent an extern record*/
typedef struct{
    char *name;
    unsigned long address;
} extern_t;

/*function to free the extern_t's content that was allocated
 * param content - pointer to extern_t object that should be freed.*/
void freeExternContent(extern_t *content);

#endif
