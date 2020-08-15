#ifndef MMN14_OUTPUT_H
#define MMN14_OUTPUT_H
#include "linked_list.h"

/*a function to write the main output file's data (.ob).
 * the function iterates the instructions and the data lists and print each of the instructions and then each of the data
 * param outputFile - the output file's FILE pointer
 * param wordList - a pointer to the instruction list
 * param dataList - a pointer to the data list*/
void writeOutputFiles(const char *baseFileName, list_t *wordList, list_t *dataList, list_t *entriesList, list_t *externsList);

/* a function to remove all the output files if they are already exist
 * param baseFileName - the base file name that is currently being parsed */
void removeOutputFiles(const char *baseFileName);

#endif
