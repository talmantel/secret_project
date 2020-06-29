#include <stdlib.h>
#include "data.h"


void freeDataContent(data_t *content){
    free(content);
}