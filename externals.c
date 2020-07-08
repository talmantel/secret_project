//
// Created by tal on 6/30/2020.
//

#include <stdlib.h>
#include "externals.h"

void freeExternalContent(external_t *content){
    free(content->name);
    free(content);
}
