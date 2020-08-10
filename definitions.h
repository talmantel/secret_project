#ifndef MMN14_DEFINITIONS_H
#define MMN14_DEFINITIONS_H

/*enum that represent possible results in the parsing process*/
typedef enum {
    ERROR = -1,  /* error*/
    SUCCESS = 0, /*success*/
    APPEND_FOR_ORIG = 1, /*need to add word for representing the origin operand*/
    APPEND_FOR_DEST = 2, /*need to add word for representing the destination operand*/
    APPEND_FOR_BOTH = 3 /*need to add two words for representing both the origin and the destination operands*/
} RESULT;

#define MAX_LINE_LENGTH 80
#define MAX_LABEL_LENGTH 31
#define START_ADDRESS 100
#define INPUT_FILE_SUFFIX ".as"
#define ENTRIES_FILE_SUFFIX ".ent"
#define EXTERNS_FILE_SUFFIX ".ext"
#define MAIN_OUTPUT_FILE_SUFFIX ".ob"

#endif //MMN14_DEFINITIONS_H

