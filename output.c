#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "output.h"
#include "definitions.h"
#include "errors.h"
#include "words.h"
#include "entries.h"
#include "extern.h"
#include "data.h"

unsigned long getWordAsLong(word_t *word);
void removeOutputFile(const char * baseFileName, char * suffix);
void getFullFileName(char ** fullFileName, const char * baseFileName, char * suffix);
void writeMainOutput(const char * baseFileName, list_t *wordList, list_t *dataList);
void writeMainOutputData(FILE *outputFile, list_t *wordList, list_t *dataList);
void writeEntriesOutput(const char * baseFileName, list_t *entriesList);
void writeEntriesOutputData(FILE *outputFile, list_t *entriesList);
void writeExternsOutput(const char * baseFileName, list_t *externalsList);
void writeExternsOutputData(FILE *outputFile, list_t *externalsList);

/*a function to write the output files.
 * param baseFileName - the base file name that is currently being parsed
 * param wordList - a pointer to the instruction list
 * param dataList - a pointer to the data list
 * param entriesList - a pointer to the entries list
 * param externsList - a pointer to the externs list */
void writeOutputFiles(const char *baseFileName, list_t *wordList, list_t *dataList, list_t *entriesList, list_t *externsList){
    /* output object file */
    writeMainOutput(baseFileName, wordList, dataList);

    /* output entries file */
    writeEntriesOutput(baseFileName, entriesList);

    /* output externs file */
    writeExternsOutput(baseFileName, externsList);

}

/*a function to write the main output file (.ob).
 * param baseFileName - the base file name that is currently being parsed
 * param wordList - a pointer to the instruction list
 * param dataList - a pointer to the data list*/
void writeMainOutput(const char * baseFileName, list_t *wordList, list_t *dataList){
    char *fullFileName;
    FILE *outputFile;

    getFullFileName(&fullFileName, baseFileName, MAIN_OUTPUT_FILE_SUFFIX);

    outputFile = fopen(fullFileName, "w");

    if(outputFile == NULL)
        printError(baseFileName, "Cannot write to file '%s'!\n", fullFileName);
    else {
        writeMainOutputData(outputFile, wordList, dataList);
        fclose(outputFile);
    }
    free(fullFileName); /*free the memory allocated by getFullFileName*/
}

/*a function to write the main output file's data (.ob).
 * the function iterates the instructions and the data lists and print each of the instructions and then each of the data
 * param outputFile - the output file's FILE pointer
 * param wordList - a pointer to the instruction list
 * param dataList - a pointer to the data list*/
void writeMainOutputData(FILE *outputFile, list_t *wordList, list_t *dataList) {
    data_word_t *data;
    node_t *currentNode;
    unsigned long wordAddress = START_ADDRESS;

    /* write file header */
    fprintf(outputFile, "%d %d\n", wordList->length, dataList->length);

    /* write instructions */
    currentNode = wordList->head;
    while(currentNode != NULL){
        fprintf(outputFile, "%07lu %06lx\n", wordAddress, getWordAsLong(currentNode->content) & 0xffffff);
        currentNode = currentNode->next;
        wordAddress++;
    }

    /* write data */
    currentNode = dataList->head;
    while(currentNode != NULL){
        data = currentNode->content;
        fprintf(outputFile, "%07lu %06lx\n", wordAddress, data->value & 0xffffff);
        currentNode = currentNode->next;
        wordAddress++;
    }
}

/*a function to write the entries output file (.ent).
 * param baseFileName - the base file name that is currently being parsed
 * param entriesList - a pointer to the entries list */
void writeEntriesOutput(const char * baseFileName, list_t *entriesList){
    char *fullFileName;
    FILE *outputFile;

    if(entriesList->length > 0) {
        getFullFileName(&fullFileName, baseFileName, ENTRIES_FILE_SUFFIX);

        outputFile = fopen(fullFileName, "w");

        if (outputFile == NULL)
            printError(baseFileName, "Cannot write to file '%s'!\n", fullFileName);
        else {
            writeEntriesOutputData(outputFile, entriesList);
            fclose(outputFile);
        }
        free(fullFileName); /*free the memory allocated by getFullFileName*/
    }
}

/*a function to write the entries output file's data (.ent).
 * the function iterates the entries list and print each of the entries
 * param outputFile - the output file's FILE pointer
 * param entriesList - a pointer to the entries list */
void writeEntriesOutputData(FILE *outputFile, list_t *entriesList) {
    entry_t *entry;
    node_t *currentNode;
    currentNode = entriesList->head;
    while(currentNode != NULL){
        entry = currentNode->content;
        fprintf(outputFile, "%s %07lu\n", entry->name, (long)entry->address);
        currentNode = currentNode->next;
    }
}

/*a function to write the externs output file (.ext).
 * param baseFileName - the base file name that is currently being parsed
 * param externsList - a pointer to the externs list */
void writeExternsOutput(const char * baseFileName, list_t *externalsList){
    char *fullFileName;
    FILE *outputFile;

    if(externalsList->length > 0) {
        getFullFileName(&fullFileName, baseFileName, EXTERNS_FILE_SUFFIX);

        outputFile = fopen(fullFileName, "w");

        if (outputFile == NULL)
            printError(baseFileName, "Cannot write to file '%s'!\n", fullFileName);
        else{
            writeExternsOutputData(outputFile, externalsList);
            fclose(outputFile);
        }
        free(fullFileName); /*free the memory allocated by getFullFileName*/
    }
}

/*a function to write the externs output file's data (.ext).
 * the function iterates the externs list and print each of the externs
 * param outputFile - the output file's FILE pointer
 * param externsList - a pointer to the externs list */
void writeExternsOutputData(FILE *outputFile, list_t *externalsList) {
    extern_t *external;
    node_t *currentNode;
    currentNode = externalsList->head;
    while(currentNode != NULL){
        external = currentNode->content;
        fprintf(outputFile, "%s %07lu\n", external->name, external->address);
        currentNode = currentNode->next;
    }
}

/* a function to remove all the output files if they are already exist
 * param baseFileName - the base file name that is currently being parsed */
void removeOutputFiles(const char *baseFileName){
    /* object file */
    removeOutputFile(baseFileName, MAIN_OUTPUT_FILE_SUFFIX);

    /* entries file */
    removeOutputFile(baseFileName, ENTRIES_FILE_SUFFIX);

    /* externs file */
    removeOutputFile(baseFileName, EXTERNS_FILE_SUFFIX);
}

/* a function to remove an output file if he is already exists
 * param baseFileName - the base file name that is currently being parsed
 * param suffix - the suffix of the file that needs to be deleted */
void removeOutputFile(const char * baseFileName, char * suffix){
    char *fullFileName;
    getFullFileName(&fullFileName, baseFileName, suffix);

    remove(fullFileName);

    free(fullFileName); /*free the memory allocated by getFullFileName*/
}

/* a function to build the full file name from the base file name and the suffix.
 * param fullFileName - a pointer to where the full file name will be stored
 * param baseFileName - the base file name that is currently being parsed
 * param suffix - the suffix of the file that needs to be deleted */
void getFullFileName(char **fullFileName, const char * baseFileName, char * suffix){
    /*base name size + suffix size + 1 for null terminator*/
    *fullFileName = malloc((strlen(baseFileName) + strlen(suffix) + 1) * sizeof(char));
    if(*fullFileName == NULL)
        handleAllocError();

    /*building the full name of the file */
    strcpy(*fullFileName, baseFileName);
    strcat(*fullFileName, suffix);
}

/* Calculate the long value of a word  and returns the result
 * param word - a pointer to the word that need to be calculated*/
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
            bitFieldValue += word->content.address->addressValue;
            bitFieldValue <<= 3U;
            bitFieldValue += word->content.address->are_type;
            break;
        case WORD_TYPE_LABEL:
            /* Labels should not be part of the output */
            break;
    }

    return bitFieldValue;
}
