#ifndef MMN14_SECOND_PASS_H
#define MMN14_SECOND_PASS_H
#include "definitions.h"
#include "linked_list.h"

RESULT secondPass(const char *fileName, list_t *symbolsList, list_t *instructionsList, list_t *entriesList,
                  list_t *externsList);

#endif //MMN14_SECOND_PASS_H
