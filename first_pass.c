#include <stdio.h>
#include <string.h>

#include "first_pass.h"
#include "parser.h"

int finishReadingLine(FILE *file);

RESULT firstPass(FILE *file, LIST symbolsList, LIST instructionsList, LIST dataList, LIST entriesList){
    char line[MAX_LINE_LENGTH+2];
    int lineNum = 1;
    RESULT result = SUCCESS;

    /* process all lines of file */
    while(fgets(line, MAX_LINE_LENGTH+2, file)) {

        /* make sure line isn't too long, and remove trailing newline character */
        char *pos = strchr(line, '\n');
        if (pos != NULL)
            *pos = '\0';
        else {
            fprintf(stderr, "Error in line %d: line longer than maximum length (%d)", lineNum, MAX_LINE_LENGTH);
            result = ERROR;
            if(finishReadingLine(file) == EOF)
                break;
        }

        if(parseLine(line, symbolsList, instructionsList, dataList, entriesList) == ERROR)
            result = ERROR;

        lineNum++;
    }

    return result;
}

/* read until end of line or EOF, returns last character read */
int finishReadingLine(FILE *file){
    int c;

    do{
        c = fgetc(file);
    }while (c != '\n' && c != EOF);

    return c;
}


