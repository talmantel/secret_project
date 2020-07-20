#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "first_pass.h"
#include "parser.h"
#include "symbols.h"
#include "data.h"
#include "entries.h"
#include "instructions.h"

int finishReadingLine(FILE *file);

void injectReal(list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList);
void injectErrors(list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList);

void correctSymbolsAddresses(list_t *symbolsList, int IC);

RESULT firstPass(FILE *file, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList){
    char line[MAX_LINE_LENGTH+3];
    int lineNum = 1;
    RESULT result = SUCCESS;
    size_t length;

    /* process all lines of file */
    while(fgets(line, MAX_LINE_LENGTH+3, file)) {

        /* make sure line isn't too long, and remove trailing newline character */
        length = strlen(line);
        if (length > 0 && line[length - 1] == '\n') {
            line[--length] = '\0';
        }

        /* TODO remove! this is to remove \r that appears in fgets result on windows only! (since it creates issues with printf) */
        if (length > 0 && line[length - 1] == '\r') {
            line[--length] = '\0';
        }


        if(length > MAX_LINE_LENGTH){
            fprintf(stderr, "Error in line %d: line longer than maximum length (%d)\n", lineNum, MAX_LINE_LENGTH);
            result = ERROR;
            if(finishReadingLine(file) == EOF)
                break;
        }
        else if(parseLine(line, lineNum, symbolsList, instructionsList, dataList, entriesList) == ERROR){
            result = ERROR;
        }

        lineNum++;
    }

    correctSymbolsAddresses(symbolsList, instructionsList->length);

    //injectErrors(symbolsList, instructionsList, dataList, entriesList);
    //injectReal(symbolsList, instructionsList, dataList, entriesList);

    return result;
}

void correctSymbolsAddresses(list_t *symbolsList, int IC){
    node_t *currentNode;
    symbol_t *symbol;
    currentNode = symbolsList->head;
    while(currentNode != NULL){
        symbol = currentNode->content;
        switch (symbol->type) {
            case CODE:
                symbol->address += START_ADDRESS;
                break;
            case DATA:
                symbol->address += START_ADDRESS + IC;
                break;
            case EXTERNAL:
                break;
        }
        currentNode = currentNode->next;
    }
}


void injectErrors(list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList){
    symbol_t *symbol;
    data_t *data;
    entry_t *entry;

    word_t *word;

    /* 1: Undefined label */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(20);
    strcpy(word->content.label->label, "LOWERCASE");
    word->content.label->lineNumber = 1;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "lowercase");
    symbol->type = CODE;
    symbol->address = 0;
    addNode(symbolsList, symbol);

    /* 2: External Entry */
    entry = malloc(sizeof(entry_t));
    entry->name = malloc(10);
    strcpy(entry->name, "external");
    entry->lineNumber = 2;
    addNode(entriesList, entry);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "external");
    symbol->type = EXTERNAL;
    symbol->address = 0;
    addNode(symbolsList, symbol);

    /* 3: Undefined Entry */
    entry = malloc(sizeof(entry_t));
    entry->name = malloc(10);
    strcpy(entry->name, "undefined");
    entry->lineNumber = 3;
    addNode(entriesList, entry);


    /* 4: external as relative */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 9;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_RELATIVE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 2;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "external");
    word->content.label->lineNumber = 4;
    word->content.label->addressing_type = ADDRESSING_TYPE_RELATIVE;
    addNode(instructionsList, word);



}

void injectReal(list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList) {
    symbol_t *symbol;
    data_t *data;
    entry_t *entry;

    word_t *word;

    /* MAIN: add r3, LIST */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 2;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->origin_reg = 3;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 1;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "LIST");
    word->content.label->lineNumber = 5;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);


    /* LOOP: prn #48 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 13;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 0;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_ADDRESS;
    word->content.address = malloc(sizeof(address_t));
    word->content.address->address = 48;
    word->content.address->are_type = A;
    addNode(instructionsList, word);


    /* lea W, r6 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 4;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->dest_reg = 6;
    word->content.instruction->funct = 0;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "W");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    /* inc r6 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 5;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->dest_reg = 6;
    word->content.instruction->funct = 3;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    /* mov r3, K */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 0;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->origin_reg = 3;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 0;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "K");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    /* sub r1 ,r4 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 2;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->origin_reg = 1;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_REGISTER;
    word->content.instruction->dest_reg = 4;
    word->content.instruction->funct = 2;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    /* bne END */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 9;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 2;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "END");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    /* cmp K, #-6 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 1;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 0;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "K");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_ADDRESS;
    word->content.address = malloc(sizeof(address_t));
    word->content.address->address = -6;
    word->content.address->are_type = A;
    addNode(instructionsList, word);

    /* bne &END */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 9;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_RELATIVE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 2;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "END");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_RELATIVE;
    addNode(instructionsList, word);


    /* dec W */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 5;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 4;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "W");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    /* jmp &LOOP */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 9;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_RELATIVE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 1;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "LOOP");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_RELATIVE;
    addNode(instructionsList, word);

    /* add L3, L3 */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 2;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_DIRECT;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 1;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "L3");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);

    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_LABEL;
    word->content.label = malloc(sizeof(label_t));
    word->content.label->label = malloc(10);
    strcpy(word->content.label->label, "L3");
    word->content.label->lineNumber = 7;
    word->content.label->addressing_type = ADDRESSING_TYPE_DIRECT;
    addNode(instructionsList, word);


    /* END: stop */
    word = malloc(sizeof(word_t));
    word->type = WORD_TYPE_INSTRUCTION;
    word->content.instruction = malloc(sizeof(instruction_t));
    word->content.instruction->opcode = 15;
    word->content.instruction->origin_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->origin_reg = 0;
    word->content.instruction->dest_addressing = ADDRESSING_TYPE_IMMEDIATE;
    word->content.instruction->dest_reg = 0;
    word->content.instruction->funct = 0;
    word->content.instruction->are_type = A;
    addNode(instructionsList, word);


    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "W");
    symbol->type = EXTERNAL;
    symbol->address = 0;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "MAIN");
    symbol->type = CODE;
    symbol->address = 100;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "LOOP");
    symbol->type = CODE;
    symbol->address = 102;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "END");
    symbol->type = CODE;
    symbol->address = 124;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "STR");
    symbol->type = DATA;
    symbol->address = 125;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "LIST");
    symbol->type = DATA;
    symbol->address = 130;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "K");
    symbol->type = DATA;
    symbol->address = 133;
    addNode(symbolsList, symbol);

    symbol = malloc(sizeof(symbol_t));
    symbol->name = malloc(10);
    strcpy(symbol->name, "L3");
    symbol->type = EXTERNAL;
    symbol->address = 0;
    addNode(symbolsList, symbol);


    data = malloc(sizeof(data_t));
    data->value = 97;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 98;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 99;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 100;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 0;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 6;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = -9;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = -100;
    addNode(dataList, data);

    data = malloc(sizeof(data_t));
    data->value = 31;
    addNode(dataList, data);



    entry = malloc(sizeof(entry_t));
    entry->name = malloc(10);
    strcpy(entry->name, "MAIN");
    addNode(entriesList, entry);

    entry = malloc(sizeof(entry_t));
    entry->name = malloc(10);
    strcpy(entry->name, "LIST");
    addNode(entriesList, entry);

}

/* read until end of line or EOF, returns last character read */
int finishReadingLine(FILE *file){
    int c;

    do{
        c = fgetc(file);
    }while (c != '\n' && c != EOF);

    return c;
}


