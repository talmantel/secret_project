#include <stdlib.h>
#include "extern.h"

/*function to free the extern_t's content that was allocated
 * param content - pointer to extern_t object that should be free.*/
void freeExternContent(extern_t *content){
    free(content->name);
    free(content);
}
