#ifndef MMN14_FIRST_PASS_H
#define MMN14_FIRST_PASS_H
#include "definitions.h"
#include "linked_list.h"

/*this function is doing the first pass part of the file's parsing and assembling.
 * the function read the file line by line and passes the line to parseLine.
 * returns ERROR if there was an error while parsing the file in the first pass, and SUCCESS otherwise
 * param fileName - the file name that is currently being parsed
 * param lineNum - the line number that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param wordList - a pointer to instruction list
 * param dataList - a pointer to data list
 * param entriesList - a pointer to entries list*/
RESULT firstPass(const char *fileName, FILE *file, list_t *symbolsList, list_t *wordList, list_t *dataList, list_t *entriesList);

#endif
