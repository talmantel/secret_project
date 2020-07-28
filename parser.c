#include "parser.h"
#include "actions.h"
#include "symbols.h"
#include "instructions.h"
#include "data.h"
#include "entries.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int addSymbolToList(list_t * symbolsList, SYMBOL_TYPE type, char * name, int address);
int addDataToList(list_t * dataList, int value, int address);
int checkIfReservedWord(char * word);
int addEntryToList(list_t * entriesList, char * name);

char * reservedWords[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not", "inc", "dec",
                          "jmp", "bne", "jsr", "red", "prn", "rt", "stop", ".string", ".entry",
                          ".data", ".external", "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "\0"};
                                                               /* null terminator to end the loop ^ */

RESULT parseLine(char *line, int lineNum, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList){
    char * token;
    int i;
    int firstWord = 1; /* flag to know if its the first word for label check*/
    int hasLabel = 0;
    char * label;

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

    while (token = strtok_r(line, " ", &line)) {
        if (firstWord) {
            if (*(token + strlen(token) - 1) == ':') { /*its a label..*/
                label = calloc(strlen(token - 1), sizeof(char));
                strncpy(label, token, strlen(token - 1));
                if (!checkIfReservedWord(label)){ /* the label is reserved word*/
                    return ERROR;
                }
                hasLabel = 1;
            }
        }
        firstWord = 0;
        if (*token == '.'){ /*start with '.' -> means its .....*/
            if (strcmp(token, ".external") == 0){
                token = strtok_r(line, " ", &line);
                if (!checkIfReservedWord(token)){ /*trying to use a reserved word*/
                    return ERROR;
                }
                if (addSymbolToList(symbolsList, EXTERNAL, token, 0) == 0){
                    return ERROR; /*duplication in symbol with different type*/
                }
                token = strtok_r(line, " ", &line);
                if (token != NULL){ /*extra after label or space inside the label -> error*/
                    return ERROR;
                }
            } else if (strcmp(token, ".entry") == 0){
                token = strtok_r(line, " ", &line);
                if (!checkIfReservedWord(token)){ /*trying to use a reserved word*/
                    return ERROR;
                }
                addEntryToList(entriesList, token); /*returning value is ignored - duplicates will be check in second pass*/
                token = strtok_r(line, " ", &line);
                if (token != NULL){ /*extra after label -> error*/
                    return ERROR;
                }
            } else if (strcmp(token, ".data") == 0){
                if (hasLabel){
                    if(addSymbolToList(symbolsList, DATA, label, dataList->length) == 0){
                        return ERROR; /*duplication in symbol with different type*/
                    }
                }
                while((token = strtok_r(line, ",", &line))){ /*splitting by ,*/
                    int i;
                    if(*(line+1) == ','){ /* there are 2 commas in a row.*/
                        return ERROR;
                    }
                    for(i = 0; i < strlen(token); i++){ /*checking if contains only numbers with leading minus or plus sign*/
                        if(!isdigit(token[i])){ /* none number parameter in .data*/
                            if ((token[i] == '-' || token[i] == '+') && i != 0) { /* has minus or plus but not in the lead*/
                                return ERROR;
                            }
                        }
                    }
                    addDataToList(dataList, atoi(token), dataList->length);
                }
            } else if (strcmp(token, ".string") == 0){
                int i;
                if (hasLabel){
                    addSymbolToList(symbolsList, DATA, label, dataList->length);
                }
                for (i =0; i < strlen(line); i++){ /*removing leading white spaces*/
                    if (!isspace(*(line+i)))
                        break;
                }
                if (*(line + i) != '"'){ /*string not starting with quotes*/
                    return ERROR;
                }
                /*get the rest of the line after .string
                 * starting from index i + 1 - i for the white spaces, 1 for the first quote */
                token = strtok_r(line + i + 1, "\"", &line);
                for(i = 0; token[i]; i++){
                    if (!isprint(token[i])){ /*check if this a printable char*/
                        return ERROR;
                    }
                    addDataToList(dataList, token[i], dataList->length);
                }
            } else { /*error - start with '.' but none of the possibilities*/
                return ERROR;
            }
            hasLabel = 0;
            break; /* already done reading the line inside the checks.*/
        } /* end checking if start with . (dot)*/
        else {
            char * command;
            char * origOper;
            char * destOper;
            RESULT result;
            int j;
            instruction_t * instruction = malloc(sizeof(instruction_t));
            strcpy(command, token);

            /*checking if there is more in the rest of the line.*/
            if (strlen(line) < 1){ /* no more  - no operands.*/
                origOper = NULL;
                destOper = NULL;
            } else { /* at least one operand*/
                token = strtok_r(line, ",", &line);

                /*checking if there is more in the rest of the line.*/
                if (strlen(line) < 1) { /* no more  - only one operand*/
                    origOper = NULL;
                    destOper = strtok_r(token, " ", &token); /*cleaning tailing spaces and checking if there was space in the middle*/
                    if (strlen(token) > 0) {  /*there was a space in the middle*/
                        return ERROR;
                    }
                } else { /* two operands */
                    if(*line == ','){ /*two commas in a row*/
                        return ERROR;
                    }
                    origOper = strtok_r(token, " ", &token); /*cleaning tailing spaces and checking if there was space in the middle*/
                    if (strlen(token) > 0) {  /*there was a space in the middle*/
                        return ERROR;
                    }
                    for (j = 0; j < strlen(line); j++){ /*removing leading white spaces from destOper*/
                        if (!isspace(*(line+j)))
                            break;
                    }
                    destOper = strtok_r(line + j, " ", &line); /*cleaning tailing spaces and checking if there was space in the middle*/
                    if (strlen(line) > 0) {  /*there was a space in the middle*/
                        return ERROR;
                    }
                }
            }
            result = setCommandParameters(command, origOper, destOper, instruction);
            if (result == ERROR){
                return ERROR;
            } else {
                word_t * word = malloc(sizeof(word_t));
                word->type = WORD_TYPE_INSTRUCTION;
                word->content.instruction = instruction;
                addNode(instructionsList, word);
                if (hasLabel){
                    addSymbolToList(symbolsList, WORD_TYPE_INSTRUCTION, label, instructionsList->length-1);
                }
                if (result == SUCCESS){ /*no need to append any extra word*/
                    return SUCCESS;
                }
                if (result & APPEND_FOR_ORIG){
                    word = malloc(sizeof(word_t));
                    if (instruction->origin_addressing == ADDRESSING_TYPE_IMMEDIATE){
                        word->type = WORD_TYPE_ADDRESS;
                        word->content.address = malloc(sizeof(address_t));
                        word->content.address->address = atoi((origOper+1));
                        word->content.address->are_type = A;
                    } else {
                        word->content.label = malloc(sizeof(label_t));
                        strcpy(word->content.label->label, (origOper));
                        word->content.label->addressing_type = instruction->origin_addressing;
                    }
                    addNode(instructionsList, word);
                }
                if (result & APPEND_FOR_DEST){
                    word = malloc(sizeof(word_t));
                    if (instruction->dest_addressing == ADDRESSING_TYPE_IMMEDIATE){
                        word->type = WORD_TYPE_ADDRESS;
                        word->content.address->address = atoi((destOper+1));
                        word->content.address->are_type = A;
                    } else {
                        word->content.label = malloc(sizeof(label_t));
                        strcpy(word->content.label->label, (destOper));
                        word->content.label->addressing_type = instruction->origin_addressing;
                    }
                    addNode(instructionsList, word);
                }
            }
        }
    }
    return SUCCESS;
}

int addSymbolToList(list_t *symbolsList, SYMBOL_TYPE type, char * name, int address){
    symbol_t * exist = search(symbolsList, compareSymbol, name);
    if (!exist){
        symbol_t *symbol = malloc(sizeof(symbol_t));
        symbol->type = type;
        strcpy(symbol->name, name);
        symbol->address = address;
        addNode(symbolsList, symbol);
        return 1;
    } else {
        if (exist->type == type){
            return 1;
        }
    }
    return 0;
}

int addEntryToList(list_t * entriesList, char * name){
    entry_t * exist = search(entriesList, compareEntry, name);
    if (!exist){
        entry_t *entry = malloc(sizeof(entry_t));
        strcpy(entry->name, name);
        addNode(entriesList, entry);
        return 1;
    }
    return 0;
}

int addDataToList(list_t * dataList, int value, int address){
    data_t * data = malloc(sizeof(data_t));
    data->value = value;
    addNode(dataList, data);
    return 1;
}

/*checking if the word is a reserved word.
 * returns 0 if yes, 1 otherwise.*/
int checkIfReservedWord(char * word){
    int i;
    for (i = 0; *reservedWords[i]; i++){
        if (strcmp(word, reservedWords[i]) == 0){
            return 0;
        }
    }
    return 1;
}
