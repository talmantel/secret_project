#ifndef MMN14_DATA_H
#define MMN14_DATA_H

typedef struct{
    unsigned long value:24;
} data_t;

void freeDataContent(data_t *content);

#endif //MMN14_DATA_H
