#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

void handleError(ERROR_TYPE error){
    switch (error) {
        case ERROR_OUT_OF_MEMORY:
            fprintf(stderr, "FATAL ERROR: Out of memory\n");
            exit(1);
    }
}

void printError(const char *fileName, long lineNum, const char *format, const void *value){
    char formatPrefixWithLine[] = "Error in file: '%s':%ld - ";
    char formatPrefix[] = "Error in file: '%s' - ";


    char *errorFormat;

    if(lineNum == -1) {
        errorFormat = malloc(strlen(formatPrefix) + strlen(format) + 1);
        strcpy(errorFormat, formatPrefix);
    }
    else {
        errorFormat = malloc(strlen(formatPrefixWithLine) + strlen(format) + 1);
        strcpy(errorFormat, formatPrefixWithLine);
    }

    strcat(errorFormat, format);

    if(lineNum == -1) {
        if (value == NULL)
            fprintf(stderr, errorFormat, fileName);
        else
            fprintf(stderr, errorFormat, fileName, value);
    }
    else{
        if (value == NULL)
            fprintf(stderr, errorFormat, fileName, lineNum);
        else
            fprintf(stderr, errorFormat, fileName, lineNum, value);
    }
    free(errorFormat);
}
