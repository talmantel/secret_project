#ifndef MMN14_PARSER_H
#define MMN14_PARSER_H

#include "linked_list.h"
#include "definitions.h"

RESULT parseLine(const char *fileName, char *line, int lineNum, list_t *symbolsList, list_t *wordList, list_t *dataList, list_t *entriesList);

#endif