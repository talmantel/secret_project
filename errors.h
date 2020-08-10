#ifndef MMN14_ERRORS_H
#define MMN14_ERRORS_H

#define ERROR_OUTPUT_FILE stdout

void handleAllocError();
void printErrorWithLine(const char *fileName, long lineNum, const char *format, const void *value);
void printError(const char *fileName, const char *format, const void *value);

#endif
