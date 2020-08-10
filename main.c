#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "definitions.h"
#include "linked_list.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output.h"
#include "data.h"
#include "entries.h"
#include "instructions.h"
#include "symbols.h"
#include "errors.h"
#include "extern.h"

void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externsList);

int main(int argc, char** argv) {

    /*linked lists to hold all the parsed data that should be outputted*/
    list_t *symbolsList;
    list_t *instructionsList;
    list_t *dataList;
    list_t *entriesList;
    list_t *externsList;

    /* process the files received as arguments - iterates over each of the arguments */
    int i = 1;
    while(i < argc){

        /* initialize dynamic storage */
        symbolsList = initLinkedList();
        instructionsList = initLinkedList();
        dataList = initLinkedList();
        entriesList = initLinkedList();
        externsList = initLinkedList();

        processFile(*(argv+i), symbolsList, instructionsList, dataList, entriesList, externsList);
        i++;

        /* free allocated memory */
        freeList(symbolsList, (void (*)(void *)) freeSymbolContent);
        freeList(instructionsList, (void (*)(void *)) freeWordContent);
        freeList(dataList, (void (*)(void *)) freeDataContent);
        freeList(entriesList, (void (*)(void *)) freeEntryContent);
        freeList(externsList, (void (*)(void *)) freeExternContent);
    }

    return 0;
}

/*
 * run assembler on a file
 * param baseFileName - base name of file to run the assembler on
 * param symbolsList - a pointer to the symbols list
 * param instructionsList - a pointer to the instructions list
 * param dataList - a pointer to the data list
 * param entriesList - a pointer to the entries list
 * param externsList - a pointer to the externs list
 */
void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externsList) {
    char *fullInputFileName;
    FILE *inputFile;

    removeOutputFiles(baseFileName); /*removing output files if they exist*/

    /*base name size + suffix size + 1 for null terminator*/
    fullInputFileName = malloc((strlen(baseFileName) + strlen(INPUT_FILE_SUFFIX) + 1) * sizeof(char));
    if(fullInputFileName == NULL)
        handleAllocError();

    /*building the full name of the input file*/
    strcpy(fullInputFileName, baseFileName);
    strcat(fullInputFileName, INPUT_FILE_SUFFIX);

    printf("\n\nOpening input file: '%s'\n", fullInputFileName);

    inputFile = fopen(fullInputFileName, "r");

    if(inputFile != NULL) {
        printf("Running first pass on input file: '%s'\n", fullInputFileName);
        if(firstPass(baseFileName, inputFile, symbolsList, instructionsList, dataList, entriesList) == SUCCESS){
            printf("Running second pass on input file: '%s'\n", fullInputFileName);
            if(secondPass(baseFileName, symbolsList, instructionsList, entriesList, externsList) == SUCCESS){
                printf("Generating output files for input file: '%s'\n", fullInputFileName);
                writeOutputFiles(baseFileName, instructionsList, dataList, entriesList, externsList);
            }

        }

        printf("Done processing input file: '%s'\n", fullInputFileName);
        fclose(inputFile);
    }
    else{
        printError(baseFileName, "Input file '%s' does not exist or cannot be opened!\n", fullInputFileName);
    }

    free(fullInputFileName);
}
