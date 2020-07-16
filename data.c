#include <stdlib.h>
#include "data.h"


void freeDataContent(void *content){
    free((data_t * )content);
}