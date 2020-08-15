#ifndef MMN14_LABELS_H
#define MMN14_LABELS_H

#include "definitions.h"

/* a function to check if a label is legal by checking:
 *      1. label is not empty.
 *      2. label starts with alpha character only.
 *      3. label is not a reserved word.
 *      4. all the label's characters are alpha numeric characters.
 *      5. the length of the label is not exceeding the length limit.
 * returns ERROR if the label is illegal. and SUCCESS otherwise.
 * param fileName - the file name that is currently being parsed
 * param lineNum - the line number that is currently being parsed
 * param label - char * (string) to be check if is legal label.*/
RESULT checkIfLegalLabel(const char *fileName, long lineNum, const char *label);

#endif
