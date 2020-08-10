#ifndef MMN14_FIRST_PASS_H
#define MMN14_FIRST_PASS_H
#include "definitions.h"
#include "linked_list.h"

RESULT firstPass(const char *fileName, FILE *file, list_t *symbolsList, list_t *instructionsList, list_t *dataList,
                 list_t *entriesList);

#endif
