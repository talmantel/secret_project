#include <ctype.h>
#include <string.h>
#include "labels.h"
#include "errors.h"

char * reservedWords[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
                          "jmp", "bne", "jsr", "red", "prn", "rts", "stop", "string", "entry",
                          "data", "extern", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

int checkIfReservedWord(const char * word);

/*checking if the word is a reserved word.
 * returns 1 if yes, 0 otherwise.*/
int checkIfReservedWord(const char * word){
    int i;
    for (i = 0; i < sizeof(reservedWords) / sizeof(char *); i++){
        if (strcmp(word, reservedWords[i]) == 0){
            return 1;
        }
    }
    return 0;
}

RESULT checkIfLegalLabel(const char *fileName, long lineNum, const char *label) {
    int i;

    if (strlen(label) == 0){
        printError(fileName, lineNum, "Empty label!\n", label);
        return ERROR;
    }

    if(!isalpha(*label)){
        printError(fileName, lineNum, "Label '%s' begins with non-alphabetic character!\n", label);
        return ERROR;
    }

    if(checkIfReservedWord(label)){
        printError(fileName, lineNum, "Label '%s' is reserved!\n", label);
        return ERROR;
    }

    for(i = 0; label[i]; i++){
        if(!isalnum(*(label+i))){
            printError(fileName, lineNum, "Label '%s' contains non-alphanumeric characters!\n", label);
            return ERROR;
        }
    }

    if (strlen(label) > MAX_LABEL_LENGTH){
        printError(fileName, lineNum, "Label '%s' is too long!\n", label);
        return ERROR;
    }



    return SUCCESS;
}