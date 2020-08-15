#ifndef MMN14_SECOND_PASS_H
#define MMN14_SECOND_PASS_H
#include "definitions.h"
#include "linked_list.h"

/*this function is doing the second pass part of the file's parsing and assembling.
 * the function corrects and adds all the addresses.
 * returns ERROR if there was an error while parsing the file in the first pass, and SUCCESS otherwise
 * param fileName - the file name that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param wordList - a pointer to instruction list
 * param entriesList - a pointer to entries list
 * param externsList - a pointer to externs list*/
RESULT secondPass(const char *fileName, list_t *symbolsList, list_t *wordList, list_t *entriesList, list_t *externsList);

#endif
