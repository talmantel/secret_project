#include "parser.h"
#include "actions.h"
#include "symbols.h"
#include "instructions.h"
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
RESULT handleCommand(const char *fileName, long lineNum, char *command, char *line, char *label, list_t *symbolsList, list_t *instructionsList);
RESULT handleInstruction(const char *fileName, long lineNum, char *instruction, char *line, char *label, list_t *symbolsList, list_t *dataList, list_t *entriesList);

RESULT handleExternInstruction(const char *fileName, long lineNum, char *line, list_t *symbolsList);
RESULT handleEntryInstruction(const char *fileName, long lineNum, char *line, list_t *entriesList);
RESULT handleDataInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList);
RESULT handleStringInstruction(const char *fileName, long lineNum, char *line, char *label, list_t *symbolsList, list_t *dataList);

RESULT parseLine(const char *fileName, char *line, int lineNum, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList) {
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
                result = handleCommand(fileName, lineNum, token, line, label, symbolsList, instructionsList);
            }
        }
    }

    free(label);
    return result;
}

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

int addDataToList(list_t * dataList, int value){
    data_word_t * data = malloc(sizeof(data_word_t));
    if(data == NULL)
        handleAllocError();
    data->value = value;
    addNode(dataList, data);
    return 1;
}

int isOnlyWhiteSpaces(const char * text){
    int i;
    for (i = 0; i < strlen(text); i++){
        if (!isspace(*(text+i))){
            return 0;
        }
    }
    return 1;
}

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

RESULT handleCommand(const char *fileName, long lineNum, char *command, char *line, char *label, list_t *symbolsList, list_t *instructionsList){
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

            destOper = strtok_r(token, " ",
                                &token); /*cleaning tailing spaces and checking if there was space in the middle*/
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
        addNode(instructionsList, word);
        if (label != NULL){
            if(addSymbolToList(symbolsList, CODE, label, instructionsList->length-1) == 0){
                printErrorWithLine(fileName, lineNum, "label '%s' is already defined and cannot be redefined!\n", label);
                return ERROR; /*duplication in symbol with different type*/
            }
        }

        if (result & APPEND_FOR_ORIG){
            word = malloc(sizeof(word_t));
            if(word == NULL)
                handleAllocError();
            if (instruction->origin_addressing == ADDRESSING_TYPE_IMMEDIATE){
                word->type = WORD_TYPE_ADDRESS;
                word->content.address = malloc(sizeof(address_t));
                if(word->content.address == NULL)
                    handleAllocError();
                word->content.address->address = atoi(origOper+1);
                word->content.address->are_type = A;
            } else {
                word->type = WORD_TYPE_LABEL;
                word->content.label = malloc(sizeof(label_t));
                if(word->content.label == NULL)
                    handleAllocError();
                word->content.label->lineNumber = lineNum;
                if(*origOper == '&')
                    origOper++;
                word->content.label->label = malloc((strlen(origOper)+1) * sizeof(char));
                if(word->content.label->label == NULL)
                    handleAllocError();
                strcpy(word->content.label->label, (origOper));
                word->content.label->addressing_type = instruction->origin_addressing;
            }
            addNode(instructionsList, word);
        }
        if (result & APPEND_FOR_DEST){
            word = malloc(sizeof(word_t));
            if(word == NULL)
                handleAllocError();
            if (instruction->dest_addressing == ADDRESSING_TYPE_IMMEDIATE){
                word->type = WORD_TYPE_ADDRESS;
                word->content.address = malloc(sizeof(address_t));
                if(word->content.address == NULL)
                    handleAllocError();
                word->content.address->address = atoi(destOper+1);
                word->content.address->are_type = A;
            } else {
                word->type = WORD_TYPE_LABEL;
                word->content.label = malloc(sizeof(label_t));
                if(word->content.label == NULL)
                    handleAllocError();
                word->content.label->lineNumber = lineNum;
                if(*destOper == '&')
                    destOper++;
                word->content.label->label = malloc((strlen(destOper)+1) * sizeof(char));
                if(word->content.label->label == NULL)
                    handleAllocError();
                strcpy(word->content.label->label, destOper);
                word->content.label->addressing_type = instruction->dest_addressing;
            }
            addNode(instructionsList, word);
        }
    }

    return SUCCESS;
}