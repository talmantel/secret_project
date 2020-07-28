#ifndef MMN14_ERRORS_H
#define MMN14_ERRORS_H

typedef enum {ERROR_OUT_OF_MEMORY} ERROR_TYPE;

void handleError(ERROR_TYPE error);
void printError(const char *fileName, long lineNum, const char *format, const void *value);

#endif //MMN14_ERRORS_H
