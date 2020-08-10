#ifndef MMN14_ERRORS_H
#define MMN14_ERRORS_H

void handleAllocError();
void printErrorWithLine(const char *fileName, long lineNum, const char *format, const void *value);
void printError(const char *fileName, const char *format, const void *value);

#endif //MMN14_ERRORS_H
