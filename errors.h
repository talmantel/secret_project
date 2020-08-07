#ifndef MMN14_ERRORS_H
#define MMN14_ERRORS_H

void handleMallocError();
void printError(const char *fileName, long lineNum, const char *format, const void *value);

#endif //MMN14_ERRORS_H
