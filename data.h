//
// Created by tal on 6/26/2020.
//

#ifndef MMN14_DATA_H
#define MMN14_DATA_H

typedef struct{
    unsigned long value:24;
} data_t;

void freeDataContent(void *content);

#endif //MMN14_DATA_H
