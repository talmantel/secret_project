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
#include "externals.h"

void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externalsList);

int main(int argc, char** argv) {

    list_t *symbolsList;
    list_t *instructionsList;
    list_t *dataList;
    list_t *entriesList;
    list_t *externalsList;

    /* process the files received as arguments */
    int i = 1;
    while(i < argc){

        /* initialize dynamic storage */
        symbolsList = initLinkedList();
        instructionsList = initLinkedList();
        dataList = initLinkedList();
        entriesList = initLinkedList();
        externalsList = initLinkedList();

        processFile(*(argv+i), symbolsList, instructionsList, dataList, entriesList, externalsList);
        i++;

        /* free allocated memory */
        freeList(symbolsList, (void (*)(void *)) freeSymbolContent);
        freeList(instructionsList, (void (*)(void *)) freeWordContent);
        freeList(dataList, (void (*)(void *)) freeDataContent);
        freeList(entriesList, (void (*)(void *)) freeEntryContent);
        freeList(externalsList, (void (*)(void *)) freeExternalContent);
    }

    return 0;
}

/*
 * run assembler on a file
 * param baseFileName - base name of file to run the assembler on
 */
void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList, list_t *externalsList) {
    char *fullInputFileName;
    FILE *inputFile;

    fullInputFileName = malloc((strlen(baseFileName) + strlen(INPUT_FILE_SUFFIX) + 1) * sizeof(char));
    if(fullInputFileName == NULL)
        handleError(ERROR_OUT_OF_MEMORY);

    strcpy(fullInputFileName, baseFileName);
    strcat(fullInputFileName, INPUT_FILE_SUFFIX);

    printf("\n\nOpening input file: '%s'\n", fullInputFileName);

    inputFile = fopen(fullInputFileName, "r");

    if(inputFile != NULL) {
        printf("Running first pass on input file: '%s'\n", fullInputFileName);
        if(firstPass(baseFileName, inputFile, symbolsList, instructionsList, dataList, entriesList) == SUCCESS){
            printf("Running second pass on input file: '%s'\n", fullInputFileName);
            if(secondPass(baseFileName, symbolsList, instructionsList, entriesList, externalsList) == SUCCESS){
                printf("Generating output files for input file: '%s'\n", fullInputFileName);
                outputFiles(baseFileName, instructionsList, dataList, entriesList, externalsList);
            }

        }

        printf("Done processing input file: '%s'\n", fullInputFileName);
        fclose(inputFile);
    }
    else{
        printError(baseFileName, -1, "Input file '%s' does not exist or cannot be opened!\n", fullInputFileName);
    }

    free(fullInputFileName);
}
