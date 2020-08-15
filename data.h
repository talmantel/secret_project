#ifndef MMN14_DATA_H
#define MMN14_DATA_H

/*struct that represent a data word structure by bitfield*/
typedef struct{
    unsigned long value;
} data_word_t;

/*function to free the data_word_t's content that was allocated
 * param content - pointer to data_word_t object that should be freed.*/
void freeDataContent(void *content);

#endif
