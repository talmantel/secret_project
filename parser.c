#include "parser.h"
#include "words.h"
#include "actions.h"
#include "symbols.h"
#include "data.h"
#include "entries.h"
#include "errors.h"
#include "labels.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int addSymbolToList(list_t * symbolsList, SYMBOL_TYPE type, char * name, int address);
int addDataToList(list_t * dataList, int value);
int addEntryToList(list_t *entriesList, char *name, long lineNum);
int isOnlyWhiteSpaces(const char * text);
RESULT handleLabel(const char *fileName, long lineNum, char *token, char **label);
RESULT handleCommand(const char *fileName, long lineNum, char *command, char *line, char *label, list_t *symbolsList, list_t *wordList);
RESULT handleInstruction(const char *fileName, long lineNum, char *instruction, char *line, char *label, list_t *symbolsList, list_t *dataList, list_t *entriesList);

RESULT handleExternInstruction(const char *fileName, long lineNum, char *line, list_t *symbolsList);
RESULT handleEntryInstruction(const char *fileName, long lineNum, char *line, list_t *entriesList);
RESULT handleDataInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList);
RESULT handleStringInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList);

void appendWord(long lineNum, ADDRESSING_TYPE addressingType, const char *oper, list_t *wordList);

char *strtok_r (char *s, const char *delim, char **save_ptr);

/*this function parses a line.
 * the function returns ERROR if any error occured and SUCCESS otherwise.
 * param fileName -
 * param line - the line string to be parsed
 * param lineNum -
 * param symbolsList - a pointer to the symbols list
 * param wordList - a pointer to the word list
 * param dataList - a pointer to the data list
 * param entriesList - a pointer to the entries list*/
RESULT parseLine(const char *fileName, char *line, long lineNum, list_t *symbolsList, list_t *wordList, list_t *dataList, list_t *entriesList) {
    char * token;
    int i;
    char * label = NULL;
    RESULT result;

    if(strlen(line) == 0)
        return SUCCESS;

    /*removing leading whitespaces chars if exist*/
    for (i = 0; i < strlen(line); i++){
        if (i == (strlen(line)-1) && isspace(*(line + i))){ /*the line contains only whitespaces*/
            return SUCCESS;
        }
        if (!isspace(*(line+i)))
            break;
    }
    if (*line == ';') /*line start with ; -> its a comment - ignoring the line.*/
        return SUCCESS;


    token = strtok_r(line, " ", &line);

    result = handleLabel(fileName, lineNum, token, &label);

    if(result != ERROR) {
        if (label != NULL && (!line || strlen(line) == 0 || isOnlyWhiteSpaces(line))) {
            printErrorWithLine(fileName, lineNum, "Label without command or instruction!\n", NULL);
            result = ERROR;
        }

        if (result != ERROR) {
            if (label != NULL) {
                token = strtok_r(line, " ", &line);
            }

            if (*token == '.') {
                result = handleInstruction(fileName, lineNum, token, line, label, symbolsList, dataList, entriesList);
            } else {
                result = handleCommand(fileName, lineNum, token, line, label, symbolsList, wordList);
            }
        }
    }

    free(label);
    return result;
}

/*a function to add the symbol to the symbols list
 * param symbolsList - a pointer to the symbols list
 * param type - the type of the symbol
 * param name - the name of the symbol
 * param address - the line address of the symbol*/
int addSymbolToList(list_t *symbolsList, SYMBOL_TYPE type, char * name, int address){
    symbol_t * exist = search(symbolsList, compareSymbol, name);
    if (!exist){
        symbol_t *symbol = malloc(sizeof(symbol_t));
        if(symbol == NULL)
            handleAllocError();
        symbol->type = type;
        symbol->name = malloc((strlen(name)+1)*sizeof(char));
        if(symbol->name == NULL)
            handleAllocError();
        strcpy(symbol->name, name);
        symbol->address = address;
        addNode(symbolsList, symbol);
        return 1;
    } else {
        if (exist->type == EXTERNAL && type == EXTERNAL){
            return 1;
        }
    }
    return 0;
}

/*a function to add the entry to the entries list
 * param entriesList - a pointer to the entries list
 * param name - the name of the entry
 * param lineNum - the line number of the entry*/
int addEntryToList(list_t *entriesList, char *name, long lineNum) {
    entry_t * exist = search(entriesList, compareEntry, name);
    if (!exist){
        entry_t *entry = malloc(sizeof(entry_t));
        if(entry == NULL)
            handleAllocError();
        entry->name = malloc((strlen(name)+1)*sizeof(char));
        if(entry->name == NULL)
            handleAllocError();
        entry->lineNumber = lineNum;
        strcpy(entry->name, name);
        addNode(entriesList, entry);
        return 1;
    }
    return 0;
}

/*a function to add the data to the data list
 * param dataList - a pointer to the data list
 * param value - the value of the data */
int addDataToList(list_t * dataList, int value){
    data_word_t * data = malloc(sizeof(data_word_t));
    if(data == NULL)
        handleAllocError();
    data->value = value;
    addNode(dataList, data);
    return 1;
}

/*this function checks if a given string contains only white spaces.
 * the function returns 1 if yes and 0 otherwise.
 * param text - the string to be checked.*/
int isOnlyWhiteSpaces(const char * text){
    int i;
    for (i = 0; i < strlen(text); i++){
        if (!isspace(*(text+i))){
            return 0;
        }
    }
    return 1;
}

/* a function to handle a label in the line.
 * the function processes the parsed line and checks if there is a label in the line.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param token  - the string that is currently being parsed
 * param label - a pointer to be updated to save the label if exist*/
RESULT handleLabel(const char *fileName, long lineNum, char *token, char **label){
    if (*(token + strlen(token) - 1) == ':') { /*its a label..*/
        *label = calloc(strlen(token), sizeof(char));
        if(*label == NULL)
            handleAllocError();
        strncpy(*label, token, strlen(token)-1);

        if(checkIfLegalLabel(fileName, lineNum, *label) == ERROR) {
            /* error printed in checkIfLegalLabel */
            return ERROR;
        }
    }
    return SUCCESS;
}

/* a function to handle a line of instruction (start with '.').
 * the function processes the parsed line and calls the perspective function according the type of the instruction.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param instruction - the instruction string to determine the type
 * param line - the rest of the line after '.data'
 * param label - the label of the line if exist
 * param symbolsList - a pointer to the symbols list
 * param dataList - a pointer to the data list
 * param entriesList - a pointer to the entries list*/
RESULT handleInstruction(const char *fileName, long lineNum, char *instruction, char *line, char *label, list_t *symbolsList, list_t *dataList, list_t *entriesList){
    if(!line || !*line){
        printErrorWithLine(fileName, lineNum, "instruction missing argument!\n", NULL);
        return ERROR;
    }

    if (strcmp(instruction, ".extern") == 0){
        handleExternInstruction(fileName, lineNum, line, symbolsList);
    } else if (strcmp(instruction, ".entry") == 0){
        handleEntryInstruction(fileName, lineNum, line, entriesList);
    } else if (strcmp(instruction, ".data") == 0){
        handleDataInstruction(fileName, lineNum, line, label, symbolsList, dataList);
    } else if (strcmp(instruction, ".string") == 0){
        handleStringInstruction(fileName, lineNum, line, label, symbolsList, dataList);
    } else { /*error - not a defined instruction */
        printErrorWithLine(fileName, lineNum, "instruction '%s' does not exist!\n", instruction);
        return ERROR;
    }

    return SUCCESS;
}

/* a function to handle a line of extern instruction (.extern).
 * the function processes the parsed line and adds the extern's symbol to the symbols list.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param line - the rest of the line after '.entry'
 * param symbolsList - a pointer to the symbols list */
RESULT handleExternInstruction(const char *fileName, long lineNum, char *line, list_t *symbolsList){
    char *token = strtok_r(line, " ", &line);

    if(checkIfLegalLabel(fileName, lineNum, token) == ERROR) {
        /* error printed in checkIfLegalLabel */
        return ERROR;
    }

    if (addSymbolToList(symbolsList, EXTERNAL, token, 0) == 0){
        printErrorWithLine(fileName, lineNum, "label '%s' is already defined and cannot be redefined as external!\n",
                           token);
        return ERROR; /*duplication in symbol with different type*/
    }
    token = strtok_r(line, " ", &line);
    if (token != NULL){ /*extra after label or space inside the label -> error*/
        printErrorWithLine(fileName, lineNum, "extra text '%s' after instruction!\n", token);
        return ERROR;
    }

    return SUCCESS;
}

/* a function to handle a line of entry instruction (.entry).
 * the function processes the parsed line and adds the entry to the entries list.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param line - the rest of the line after '.entry'
 * param entriesList - a pointer to the entries list */
RESULT handleEntryInstruction(const char *fileName, long lineNum, char *line, list_t *entriesList){
    char *token = strtok_r(line, " ", &line);

    if(checkIfLegalLabel(fileName, lineNum, token) == ERROR) {
        /* error printed in checkIfLegalLabel */
        return ERROR;
    }

    addEntryToList(entriesList, token, lineNum); /*returning value is ignored - duplicates will be check in second pass*/
    token = strtok_r(line, " ", &line);
    if (token != NULL){ /*extra after label -> error*/
        printErrorWithLine(fileName, lineNum, "extra text '%s' after instruction!\n", token);
        return ERROR;
    }

    return SUCCESS;
}

/* a function to handle a line of data instruction (.data).
 * the function processes the parsed line and adds the data to the data list.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param line - the rest of the line after '.data'
 * param label - the label of the line if exist
 * param symbolsList - a pointer to the symbols list
 * param dataList - a pointer to the data list */
RESULT handleDataInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList){
    char * token;
    int i;

    if (label != NULL){
        if(addSymbolToList(symbolsList, DATA, label, dataList->length) == 0){
            printErrorWithLine(fileName, lineNum, "label '%s' is already defined and cannot be redefined!\n", label);
            return ERROR; /*duplication in symbol with different type*/
        }
    }
    if (*line == ','){ /*the data starting with a comma*/
        printErrorWithLine(fileName, lineNum, "Illegal comma!\n", NULL);
        return ERROR;
    }
    for (i=0; i < strlen(line); i++){  /*checking if there are multiple consecutive commas*/
        if (*(line+i) == ',' && i < (strlen(line) - 1)){
            if (*(line + i + 1) == ','){
                printErrorWithLine(fileName, lineNum, "multiple consecutive commas!\n", NULL);
                return ERROR;
            }
        }
    }

    while((token = strtok_r(line, ",", &line))){ /*splitting by ,*/
        if(line && strlen(line) < 1){ /*nothing was after the comma*/
            printErrorWithLine(fileName, lineNum, "Missing operand!\n", NULL);
            return ERROR;
        }

        for (; *token; token++){ /*removing leading white spaces*/
            if (!isspace(*token))
                break;
        }

        for(i = 0; i < strlen(token); i++){ /*checking if contains only numbers with leading minus or plus sign*/
            if(!isdigit(token[i])){ /* none number parameter in .data*/
                if(isspace(token[i])){
                    for(; i < strlen(token); i++){
                        if(!isspace(token[i])){
                            printErrorWithLine(fileName, lineNum, "Missing comma at '%s'!\n", token + i);
                            return ERROR;
                        }
                    }
                }
                else if (((token[i] == '-' || token[i] == '+') && i != 0) || (token[i] != '-' && token[i] != '+')) {
                    /* has minus or plus but not in the lead or not a number or minus/plus sign*/
                    printErrorWithLine(fileName, lineNum, "argument '%s' is not a number!\n", token);
                    return ERROR;
                }
            }
        }
        addDataToList(dataList, strtol(token, NULL, 10));
    }

    return SUCCESS;
}

/* a function to handle a line of string instruction (.string).
 * the function processes the parsed line and adds the string to the data list.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param line - the rest of the line after '.string'
 * param label - the label of the line if exist
 * param symbolsList - a pointer to the symbols list
 * param dataList - a pointer to the data list */
RESULT handleStringInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList){
    int i, j;

    if (label != NULL){
        if(addSymbolToList(symbolsList, DATA, label, dataList->length) == 0){
            printErrorWithLine(fileName, lineNum, "label '%s' is already defined and cannot be redefined!\n", label);
            return ERROR; /*duplication in symbol with different type*/
        }
    }

    for (i =0; i < strlen(line); i++){ /*removing leading white spaces*/
        if (!isspace(*(line+i)))
            break;
    }
    if (*(line + i) != '"'){ /*string not starting with quotes*/
        printErrorWithLine(fileName, lineNum, "missing start quotes before '%s'!\n", line + i);
        return ERROR;
    }
    /*find the last occurrence of " and checks if there non-white chars after it
     * it will also catch if there is only starting " with out ending "*/
    for (j = strlen(line) - 1; j >= 0; j--){
        if (*(line + j) == '\"')
            break;
        if (!isspace(*(line + j))){
            printErrorWithLine(fileName, lineNum, "no end quotes\n", NULL);
            return ERROR;
        }
    }
    /* add string to data */
    for(i++; i < j; i++){
        if (!isprint(line[i])){ /*check if this a printable char*/
            printErrorWithLine(fileName, lineNum, "non printable character '%c' in string argument!\n", line + i);
            return ERROR;
        }
        addDataToList(dataList, line[i]);
    }
    addDataToList(dataList, '\0');

    return SUCCESS;
}

/* a function to handle a line of command (action).
 * the function processes the parsed line and adds words accordingly.
 * the function returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the name of the file that is being currently parsed.
 * param lineNum - the line number that is currently being parsed.
 * param command - the command parsed from the line.
 * param line - the rest of the line after the command
 * param label - the label of the line if exist
 * param symbolsList - a pointer to the symbols list
 * param wordList - a pointer to the word list */
RESULT handleCommand(const char *fileName, long lineNum, char *command, char *line, char *label, list_t *symbolsList, list_t *wordList){
    char * token;
    char * origOper;
    char * destOper;
    RESULT result;
    int j;
    instruction_t * instruction;

    /*checking if there is more in the rest of the line.*/
    if (!line || strlen(line) < 1){ /* no more  - no operands.*/
        origOper = NULL;
        destOper = NULL;
    } else { /* at least one operand*/
        token = strtok_r(line, ",", &line);

        /*checking if there is more in the rest of the line.*/
        if (!line || strlen(line) < 1) { /* no more  - only one operand*/
            origOper = NULL;
            destOper = strtok_r(token, " ", &token); /*cleaning tailing spaces and checking if there was space in the middle*/
            if (token && strlen(token) > 0) {  /*there was a space in the middle*/
                printErrorWithLine(fileName, lineNum, "extra text '%s' after instruction!\n", token);
                return ERROR;
            }
        } else { /* two operands */

            for (j = 0; j < strlen(line); j++) { /*removing leading white spaces from destOper*/
                if (!isspace(*(line + j)))
                    break;
            }
            if(*(line+j) == ','){ /*two commas in a row*/
                printErrorWithLine(fileName, lineNum, "multiple consecutive commas!\n", NULL);
                return ERROR;
            }

            origOper = strtok_r(token, " ", &token); /*cleaning tailing spaces and checking if there was space in the middle*/

            if (token && strlen(token) > 0 && !isOnlyWhiteSpaces(token)) {  /*there was a space in the middle*/
                printErrorWithLine(fileName, lineNum, "Missing comma at '%s'!\n", token);
                return ERROR;
            }

            token = strtok_r(line + j, ",", &line); /*checking if entered more than 2 operands*/
            if (line && strlen(line) > 0) {  /*there was a comma in the middle*/
                printErrorWithLine(fileName, lineNum, "too many operands. the maximum number is two!\n", NULL);
                return ERROR;
            }

            destOper = strtok_r(token, " ", &token); /*cleaning tailing spaces and checking if there was space in the middle*/
            if (token && strlen(token) > 0) {  /*there was a space in the middle*/
                printErrorWithLine(fileName, lineNum, "extra text '%s' after instruction!\n", token);
                return ERROR;
            }
        }
    }

    instruction = malloc(sizeof(instruction_t));
    if(instruction == NULL)
        handleAllocError();
    result = setCommandParameters(fileName, lineNum, command, origOper, destOper, instruction);
    if (result == ERROR){
        /* error was printed already*/
        free(instruction);
        return ERROR;
    } else {
        word_t *word = malloc(sizeof(word_t));
        if(word == NULL)
            handleAllocError();
        word->type = WORD_TYPE_INSTRUCTION;
        word->content.instruction = instruction;
        addNode(wordList, word);
        if (label != NULL){
            if(addSymbolToList(symbolsList, CODE, label, wordList->length-1) == 0){
                printErrorWithLine(fileName, lineNum, "label '%s' is already defined and cannot be redefined!\n", label);
                return ERROR; /*duplication in symbol with different type*/
            }
        }

        if (result & APPEND_FOR_ORIG){
            appendWord(lineNum, instruction->origin_addressing, origOper, wordList);
        }
        if (result & APPEND_FOR_DEST){
            appendWord(lineNum, instruction->dest_addressing, destOper, wordList);
        }
    }

    return SUCCESS;
}

/* function to append the extra words to the list when needed.
 * param lineNum - the line number that is currently being parsed.
 * param addressingType - the addressing type of the operand
 * param oper - the operand that need to be added as a word to the list.
 * param wordList - a pointer to the word list */
void appendWord(long lineNum, ADDRESSING_TYPE addressingType, const char *oper, list_t *wordList){
    word_t *word = malloc(sizeof(word_t));
    if(word == NULL)
        handleAllocError();
    if (addressingType == ADDRESSING_TYPE_IMMEDIATE){
        word->type = WORD_TYPE_ADDRESS;
        word->content.address = malloc(sizeof(address_t));
        if(word->content.address == NULL)
            handleAllocError();
        word->content.address->addressValue = atoi(oper + 1);
        word->content.address->are_type = A;
    } else {
        word->type = WORD_TYPE_LABEL;
        word->content.label = malloc(sizeof(label_t));
        if(word->content.label == NULL)
            handleAllocError();
        word->content.label->lineNumber = lineNum;
        if(*oper == '&')
            oper++;
        word->content.label->labelName = malloc((strlen(oper) + 1) * sizeof(char));
        if(word->content.label->labelName == NULL)
            handleAllocError();
        strcpy(word->content.label->labelName, oper);
        word->content.label->addressing_type = addressingType;
    }
    addNode(wordList, word);
}


/* function to cut a string by a delimiter.
 * the function ignores the delimiter if its at the start of s.
 * the function then finds the next delimiter in s and cut the string there to 2 parts by replacing the delimiter with a null terminator.
 * then updates the save_ptr to the start of the second part of the cutted string.
 * the function returns a pointer to the first part of the cutted string.
 * param s - a pointer to the string to be splitted.
 * param delim - the delimiter that the string should be cutted by.
 * param save_ptr - a pointer to save the start of the second part of the cutted string. */
char *strtok_r(char *s, const char *delim, char **save_ptr){
    char *end;

    if (s == NULL){
        if (*save_ptr == NULL) /*entered NULL or reached the end of the string*/
            return NULL;
        s = *save_ptr;
    }

    if (*s == '\0'){
        *save_ptr = s;
        return NULL;
    }

    /* Scan leading delimiters and ignores them  */
    s += strspn (s, delim);
    if (*s == '\0'){
        *save_ptr = s;
        return NULL;
    }

    /* Find the end of the token.  */
    end = s + strcspn (s, delim);

    if (*end == '\0'){
        *save_ptr = NULL;
        return s;
    }

    /* replace the delimiter with a null terminator and make *SAVE_PTR point past it.  */
    *end = '\0';
    *save_ptr = end + 1;
    return s;
}