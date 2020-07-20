#include "parser.h"
#include "actions.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int addSymbolToList(list_t * symbolsList, SYMBOL_TYPE type, char * name, int address);
int addDataToList(list_t * dataList, int value, int address);

RESULT parseLine(char *line, int lineNum, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList){
    static int DC = 0;
    static int IC = 0;
    char * token;
    int i;
    int wordCount = 1;
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
        if (wordCount == 1) {
            if (*(token + strlen(token) - 1) == ':') { /*its a label..*/
                label = calloc(strlen(token - 1), sizeof(char));
                strncpy(label, token, strlen(token - 1));
                hasLabel = 1;
            }
        }
        if (*token == '.'){ /*start with '.' -> means its .....*/
            if (strcmp(token, ".external") == 0){
                token = strtok_r(line, " ", &line);
                addSymbolToList(symbolsList, EXTERNAL, token, 0);
                token = strtok_r(line, " ", &line);
                if (token != NULL){ /*extra after label -> error*/
                    return ERROR;
                }
            } else if (strcmp(token, ".entry") == 0){
                token = strtok_r(line, " ", &line);
                if (search(entriesList, compareEntry, token)){
                    entry_t *entry = malloc(sizeof(entry_t));
                    strcpy(entry->name, token);
                    addNode(entriesList, entry);
                }
                token = strtok_r(line, " ", &line);
                if (token != NULL){ /*extra after label -> error*/
                    return ERROR;
                }
            } else if (strcmp(token, ".data") == 0){
                if (hasLabel){
                    addSymbolToList(symbolsList, DATA, label, 0);
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
                    addDataToList(dataList, atoi(token), DC);
                    DC++;
                }
            } else if (strcmp(token, ".string") == 0){
                int i;
                if (hasLabel){
                    addSymbolToList(symbolsList, DATA, label, 0);
                }
                token = strtok_r(line, " ", &line); /*get the rest of the line after .string*/
                for(i = 0; token[i]; i++){
                    if (!isprint(token[i])){ /*check if this a printable char*/
                        return ERROR;
                    }
                    addDataToList(dataList, token[i], DC);
                    DC++;
                }
            } else { /*error - start with '.' but not one of the possibilities*/
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
                IC++;
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
                    IC++;
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
                    IC++;
                }
            }
        }
    }
    return SUCCESS;
}

int addSymbolToList(list_t *symbolsList, SYMBOL_TYPE type, char * name, int address){
    if (search(symbolsList, compareSymbol, name)){
        symbol_t *symbol = malloc(sizeof(symbol_t));
        symbol->type = type;
        strcpy(symbol->name, name);
        symbol->address = address;
        addNode(symbolsList, symbol);
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
