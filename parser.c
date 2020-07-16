#include "parser.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

int addSymbolToList(list_t * symbolList, SYMBOL_TYPE type, char * name, int address);
int addDataToList(list_t * dataList, int value, int address);
int DC = 0;

RESULT parseLine(char *line, int lineNum, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList){
    char c;
    char * token;
    int i;
    int wordCount = 1;
    int errorFlag = 0;
    int hasLabel = 0;
    char * label;


    if (*line == ';') /*line start with ; -> its a comment - ignoring the line.*/
        return SUCCESS;

    /*removing leading whitespaces chars if exist*/
    for (i = 0; i < strlen(line); i++){
        if (i == (strlen(line)-1) && isspace(*(line + i))){ /*the line contains only whitespaces*/
            return SUCCESS;
        }
        if (!isspace(*(line+i)))
            break;
    }

    while (token = strtok_r(line, " ", &line)) {
        if (wordCount == 1) {
            if (*(token + strlen(token) - 1) == ':') { /*its a label..*/
                label = calloc(strlen(token - 1), sizeof(char));
                strncpy(label, token, strlen(token - 1));
                hasLabel = 1;
            }
        }
        if (*token == "."){ /*start with '.' -> means its .....*/
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
                    entry->address = 0; /*not sure...*/
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
                     addDataToList(dataList, atoi(token), DC);
                     DC++;
                }
            } else if (strcmp(token, ".string") == 0){
                if (hasLabel){
                    addSymbolToList(symbolsList, DATA, label, 0);
                }
                int i;
                token = strtok_r(line, " ", &line);
                for(i = 0; token[i]; i++){
                    addDataToList(dataList, token[i], DC);
                    DC++;
                }
            } else { /*error*/
                return ERROR;
            }
            hasLabel = 0;
            break; /* already done reading the line inside the checks.*/
        } /* end checking if start with . (dot)*/
        else {
            if (token){

            }
        }
    }
    for (; i < strlen(line); i++){
        if (*(line+i) == '\n') { /*reached enf of line*/
            if (errorFlag)
                return ERROR;
            return SUCCESS;
        }


    }
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
