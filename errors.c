#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "errors.h"

/*a function to handle a memory allocating error.
 * when this error occurs - print an error to stderr and exit the program*/
void handleAllocError() {
    fprintf(ERROR_OUTPUT_FILE, "FATAL ERROR: Out of memory\n");
    exit(1);
}

/*a function to print an assembler's errors while trying to parse the input files.
 * param fileName - the file name that is currently being parsed
 * param lineNum - the line number that is currently being parsed
 * param format - a string format that should be passed to the print function
 * param value - a value to be added to the error string. can be NULL if none is needed, if inserted should be consider
 *               in format param as part of the string.*/
void printErrorWithLine(const char *fileName, long lineNum, const char *format, const void *value){
    char formatPrefixWithLine[] = "Error in file: '%s':%ld - ";
    char *errorFormat;

    errorFormat = malloc(strlen(formatPrefixWithLine) + strlen(format) + 1);
    if(errorFormat == NULL)
        handleAllocError();
    strcpy(errorFormat, formatPrefixWithLine);

    strcat(errorFormat, format);

    if (value == NULL)
        fprintf(ERROR_OUTPUT_FILE, errorFormat, fileName, lineNum);
    else
        fprintf(ERROR_OUTPUT_FILE, errorFormat, fileName, lineNum, value);

    free(errorFormat);
}

/*a function to print an assembler's errors while trying to parse the input files.
 * param fileName - the file name that is currently being parsed
 * param format - a string format that should be passed to the print function
 * param value - a value to be added to the error string. can be NULL if none is needed, if inserted should be consider
 *               in format param as part of the string.*/
void printError(const char *fileName, const char *format, const void *value){
    char formatPrefix[] = "Error in file: '%s' - ";
    char *errorFormat;

    errorFormat = malloc(strlen(formatPrefix) + strlen(format) + 1);
    if(errorFormat == NULL)
        handleAllocError();
    strcpy(errorFormat, formatPrefix);

    if (value == NULL)
        fprintf(ERROR_OUTPUT_FILE, errorFormat, fileName);
    else
        fprintf(ERROR_OUTPUT_FILE, errorFormat, fileName, value);

    free(errorFormat);
}
