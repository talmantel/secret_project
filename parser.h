#ifndef MMN14_PARSER_H
#define MMN14_PARSER_H

#include "linked_list.h"
#include "definitions.h"

#define WHITESPACES " \t"

/*this function parses a line.
 * the function returns ERROR if any error occurred and SUCCESS otherwise.
 * param fileName -
 * param line - the line string to be parsed
 * param lineNum -
 * param symbolsList - a pointer to the symbols list
 * param wordList - a pointer to the word list
 * param dataList - a pointer to the data list
 * param entriesList - a pointer to the entries list*/
RESULT parseLine(const char *fileName, char *line, long lineNum, list_t *symbolsList, list_t *wordList, list_t *dataList, list_t *entriesList);

#endif