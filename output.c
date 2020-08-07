#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "definitions.h"
#include "errors.h"
#include "instructions.h"
#include "entries.h"
#include "externals.h"
#include "data.h"

void writeMainOutput(FILE *outputFile, list_t *instructionsList, list_t *dataList);
void writeEntriesOutput(FILE *outputFile, list_t *entriesList);
void writeExternalsOutput(FILE *outputFile, list_t *externalsList);
unsigned long getWordAsLong(word_t *word);

void outputFiles(const char *baseFileName, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externalsList){
    char *currentFileName;
    FILE *outputFile;

    /* output object file */
    currentFileName = malloc((strlen(baseFileName) + strlen(MAIN_OUTPUT_FILE_SUFFIX) + 1) * sizeof(char));
    if(currentFileName == NULL)
        handleMallocError();

    strcpy(currentFileName, baseFileName);
    strcat(currentFileName, MAIN_OUTPUT_FILE_SUFFIX);

    outputFile = fopen(currentFileName, "w");

    if(outputFile == NULL)
        printError(baseFileName, -1, "Cannot write to file '%s'!\n", currentFileName);
    else {
        writeMainOutput(outputFile, instructionsList, dataList);
        fclose(outputFile);
    }

    /* output entries file */
    if(entriesList->length > 0) {
        currentFileName = realloc(currentFileName, (strlen(baseFileName) + strlen(ENTRIES_FILE_SUFFIX) + 1) * sizeof(char));
        if (currentFileName == NULL)
            handleMallocError();

        strcpy(currentFileName, baseFileName);
        strcat(currentFileName, ENTRIES_FILE_SUFFIX);

        outputFile = fopen(currentFileName, "w");

        if (outputFile == NULL)
            printError(baseFileName, -1, "Cannot write to file '%s'!\n", currentFileName);
        else {
            writeEntriesOutput(outputFile, entriesList);
            fclose(outputFile);
        }
    }

    /* output externals file */
    if(externalsList->length > 0) {
        currentFileName = realloc(currentFileName, (strlen(baseFileName) + strlen(EXTERNALS_FILE_SUFFIX) + 1) * sizeof(char));
        if (currentFileName == NULL)
            handleMallocError();

        strcpy(currentFileName, baseFileName);
        strcat(currentFileName, EXTERNALS_FILE_SUFFIX);

        outputFile = fopen(currentFileName, "w");

        if (outputFile == NULL)
            printError(baseFileName, -1, "Cannot write to file '%s'!\n", currentFileName);
        else{
            writeExternalsOutput(outputFile, externalsList);
            fclose(outputFile);
        }
    }

    free(currentFileName);
}

void removeFiles(const char *baseFileName){
    char *currentFileName;

    /* object file */
    currentFileName = malloc((strlen(baseFileName) + strlen(MAIN_OUTPUT_FILE_SUFFIX) + 1) * sizeof(char));
    if(currentFileName == NULL)
        handleMallocError();

    strcpy(currentFileName, baseFileName);
    strcat(currentFileName, MAIN_OUTPUT_FILE_SUFFIX);
    remove(currentFileName);

    /* entries file */
    currentFileName = realloc(currentFileName, (strlen(baseFileName) + strlen(ENTRIES_FILE_SUFFIX) + 1) * sizeof(char));
    if(currentFileName == NULL)
        handleMallocError();

    strcpy(currentFileName, baseFileName);
    strcat(currentFileName, ENTRIES_FILE_SUFFIX);
    remove(currentFileName);

    /* externals file */
    currentFileName = realloc(currentFileName, (strlen(baseFileName) + strlen(EXTERNALS_FILE_SUFFIX) + 1) * sizeof(char));
    if(currentFileName == NULL)
        handleMallocError();

    strcpy(currentFileName, baseFileName);
    strcat(currentFileName, EXTERNALS_FILE_SUFFIX);
    remove(currentFileName);

    free(currentFileName);
}

void writeMainOutput(FILE *outputFile, list_t *instructionsList, list_t *dataList) {
    data_t *data;
    node_t *currentNode;
    unsigned long wordAddress = START_ADDRESS;

    /* write file header */
    fprintf(outputFile, "%d %d\n", instructionsList->length, dataList->length);

    /* write instructions */
    currentNode = instructionsList->head;
    while(currentNode != NULL){
        fprintf(outputFile, "%07lu %06lx\n", wordAddress, getWordAsLong(currentNode->content) & 0xffffff);
        currentNode = currentNode->next;
        wordAddress++;
    }

    /* write data */
    currentNode = dataList->head;
    while(currentNode != NULL){
        data = currentNode->content;
        fprintf(outputFile, "%07lu %06lx\n", wordAddress, data->value);
        currentNode = currentNode->next;
        wordAddress++;
    }
}


void writeEntriesOutput(FILE *outputFile, list_t *entriesList) {
    entry_t *entry;
    node_t *currentNode;
    currentNode = entriesList->head;
    while(currentNode != NULL){
        entry = currentNode->content;
        fprintf(outputFile, "%s %07lu\n", entry->name, (long)entry->address);
        currentNode = currentNode->next;
    }
}

void writeExternalsOutput(FILE *outputFile, list_t *externalsList) {
    external_t *external;
    node_t *currentNode;
    currentNode = externalsList->head;
    while(currentNode != NULL){
        external = currentNode->content;
        fprintf(outputFile, "%s %07lu\n", external->name, external->address);
        currentNode = currentNode->next;
    }
}


/* Calculate the long value of a word */
unsigned long getWordAsLong(word_t *word) {
    unsigned long bitFieldValue = 0;
    switch (word->type) {
        case WORD_TYPE_INSTRUCTION:
            bitFieldValue += word->content.instruction->opcode;
            bitFieldValue <<= 2U;
            bitFieldValue += word->content.instruction->origin_addressing;
            bitFieldValue <<= 3U;
            bitFieldValue += word->content.instruction->origin_reg;
            bitFieldValue <<= 2U;
            bitFieldValue += word->content.instruction->dest_addressing;
            bitFieldValue <<= 3U;
            bitFieldValue += word->content.instruction->dest_reg;
            bitFieldValue <<= 5U;
            bitFieldValue += word->content.instruction->funct;
            bitFieldValue <<= 3U;
            bitFieldValue += word->content.instruction->are_type;
            break;
        case WORD_TYPE_ADDRESS:
            bitFieldValue += word->content.address->address;
            bitFieldValue <<= 3U;
            bitFieldValue += word->content.address->are_type;
            break;
        case WORD_TYPE_LABEL:
            /* Labels should not be part of the output */
            break;
    }

    return bitFieldValue;
}
