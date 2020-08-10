#include <ctype.h>
#include <string.h>
#include "labels.h"
#include "errors.h"

/*an array of reserved words that can't be used as label*/
char * reservedWords[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
                          "jmp", "bne", "jsr", "red", "prn", "rts", "stop", ".string", ".entry",
                          ".data", ".extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

int checkIfReservedWord(const char * word);

/*checking if the word is a reserved word.
 * returns 1 if yes, 0 otherwise.
 * param word - char * (string) to be check if is reserved word.*/
int checkIfReservedWord(const char * word){
    int i;
    for (i = 0; i < sizeof(reservedWords) / sizeof(char *); i++){
        if (strcmp(word, reservedWords[i]) == 0){
            return 1;
        }
    }
    return 0;
}

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
RESULT checkIfLegalLabel(const char *fileName, long lineNum, const char *label) {
    int i;

    if (strlen(label) == 0){
        printErrorWithLine(fileName, lineNum, "Empty label!\n", label);
        return ERROR;
    }

    if(!isalpha(*label)){
        printErrorWithLine(fileName, lineNum, "Label '%s' begins with non-alphabetic character!\n", label);
        return ERROR;
    }

    if(checkIfReservedWord(label)){
        printErrorWithLine(fileName, lineNum, "Label '%s' is reserved!\n", label);
        return ERROR;
    }

    for(i = 0; label[i]; i++){
        if(!isalnum(*(label+i))){
            printErrorWithLine(fileName, lineNum, "Label '%s' contains non-alphanumeric characters!\n", label);
            return ERROR;
        }
    }

    if (strlen(label) > MAX_LABEL_LENGTH){
        printErrorWithLine(fileName, lineNum, "Label '%s' is too long!\n", label);
        return ERROR;
    }
    return SUCCESS;
}