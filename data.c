#include <stdlib.h>
#include "data.h"

/*function to free the data_word_t's content that was allocated
 * param content - pointer to data_word_t object that should be free.*/
void freeDataContent(void *content){
    free((data_word_t * )content);
}