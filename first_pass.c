#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "first_pass.h"
#include "parser.h"
#include "symbols.h"
#include "errors.h"

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)

int finishReadingLine(FILE *file);

void correctSymbolsAddresses(list_t *symbolsList, int IC);

RESULT firstPass(const char *fileName, FILE *file, list_t *symbolsList, list_t *instructionsList, list_t *dataList,
                 list_t *entriesList) {
    char line[MAX_LINE_LENGTH+3];
    int lineNum = 1;
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
            printError(fileName, lineNum, "Line longer than maximum length %s!\n", STR(MAX_LINE_LENGTH));
            result = ERROR;
            if(finishReadingLine(file) == EOF)
                break;
        }
        else if(parseLine(fileName, line, lineNum, symbolsList, instructionsList, dataList, entriesList) == ERROR){
            /* error was printed in parseLine*/
            result = ERROR;
        }

        lineNum++;
    }

    correctSymbolsAddresses(symbolsList, instructionsList->length);

    return result;
}

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

/* read until end of line or EOF, returns last character read */
int finishReadingLine(FILE *file){
    int c;

    do{
        c = fgetc(file);
    }while (c != '\n' && c != EOF);

    return c;
}


