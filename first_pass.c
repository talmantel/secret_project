#include <stdio.h>
#include <string.h>
#include "first_pass.h"
#include "parser.h"
#include "symbols.h"
#include "errors.h"

/*helper macros to convert input to string*/
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

int finishReadingLine(FILE *file);
void correctSymbolsAddresses(list_t *symbolsList, int IC);

/*this function is doing the first pass part of the file's parsing and assembling.
 * the function read the file line by line and passes the line to parseLine.
 * returns ERROR if there was an error while parsing the file in the first pass, and SUCCESS otherwise
 * param fileName - the file name that is currently being parsed
 * param lineNum - the line number that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param wordList - a pointer to instruction list
 * param dataList - a pointer to data list
 * param entriesList - a pointer to entries list*/
RESULT firstPass(const char *fileName, FILE *file, list_t *symbolsList, list_t *wordList, list_t *dataList,
                 list_t *entriesList) {
    char line[MAX_LINE_LENGTH+3];
    long lineNum = 1;
    RESULT result = SUCCESS;
    size_t length;

    /* process all lines of file */
    while(fgets(line, MAX_LINE_LENGTH+3, file)) {

        /* make sure line isn't too long, and remove trailing newline character */
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[--length] = '\0';
        }

        /* TODO remove! this is to remove \r that appears in fgets result on windows only! (since it creates issues with printf) */
        if (length > 0 && line[length - 1] == '\r') {
            line[--length] = '\0';
        }


        if(length > MAX_LINE_LENGTH){
            printErrorWithLine(fileName, lineNum, "Line longer than maximum length %s!\n", STR(MAX_LINE_LENGTH));
            result = ERROR;
            if(finishReadingLine(file) == EOF)
                break;
        }
        else if(parseLine(fileName, line, lineNum, symbolsList, wordList, dataList, entriesList) == ERROR){
            /* error was printed in parseLine*/
            result = ERROR;
        }

        lineNum++;
    }

    correctSymbolsAddresses(symbolsList, wordList->length);

    return result;
}

/*a function that corrects the symbols' addresses in the symbols' list, at the end of the first pass.
 * param symbolsList - a pointer to the symbols list.
 * param IC - the instruction counter*/
void correctSymbolsAddresses(list_t *symbolsList, int IC){
    node_t *currentNode;
    symbol_t *symbol;
    currentNode = symbolsList->head;
    while(currentNode != NULL){
        symbol = currentNode->content;
        switch (symbol->type) {
            case CODE:
                symbol->address += START_ADDRESS;
                break;
            case DATA:
                symbol->address += START_ADDRESS + IC;
                break;
            case EXTERNAL:
                break;
        }
        currentNode = currentNode->next;
    }
}

/* read until end of line or EOF, returns last character read
 * param file - a pointer to FILE object to be read.*/
int finishReadingLine(FILE *file){
    int c;

    do{
        c = fgetc(file);
    }while (c != '\n' && c != EOF);

    return c;
}


