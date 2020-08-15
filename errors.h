#ifndef MMN14_ERRORS_H
#define MMN14_ERRORS_H

#define ERROR_OUTPUT_FILE stdout

/*a function to handle a memory allocating error.
 * when this error occurs - print an error and exit the program*/
void handleAllocError();

/*a function to print an assembler's errors while trying to parse the input files.
 * param fileName - the file name that is currently being parsed
 * param lineNum - the line number that is currently being parsed
 * param format - a string format that should be passed to the print function
 * param value - a value to be added to the error string. can be NULL if none is needed, if inserted should be consider
 *               in format param as part of the string.*/
void printErrorWithLine(const char *fileName, long lineNum, const char *format, const void *value);

/*a function to print an assembler's errors while trying to parse the input files.
 * param fileName - the file name that is currently being parsed
 * param format - a string format that should be passed to the print function
 * param value - a value to be added to the error string. can be NULL if none is needed, if inserted should be consider
 *               in format param as part of the string.*/
void printError(const char *fileName, const char *format, const void *value);

#endif
