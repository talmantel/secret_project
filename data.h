#ifndef MMN14_DATA_H
#define MMN14_DATA_H

/*struct that represent a data word structure by bitfield*/
typedef struct{
    unsigned long value;
} data_word_t;

void freeDataContent(void *content);

#endif
