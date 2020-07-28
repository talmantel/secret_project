#ifndef MMN14_PARSER_H
#define MMN14_PARSER_H

#include "linked_list.h"
#include "definitions.h"


RESULT parseLine(char *line, int lineNum, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList);

#endif //MMN14_PARSER_H