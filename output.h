#ifndef MMN14_OUTPUT_H
#define MMN14_OUTPUT_H
#include "linked_list.h"

void outputFiles(const char *baseFileName, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externalsList);
void removeFiles(const char *baseFileName);

#endif
