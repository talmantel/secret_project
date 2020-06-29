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

void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList);

int main(int argc, char** argv) {

    list_t *symbolsList;
    list_t *instructionsList;
    list_t *dataList;
    list_t *entriesList;

    /* process the files received as arguments */
    int i = 1;
    while(i < argc){

        /* initialize dynamic storage */
        symbolsList = initLinkedList();
        instructionsList = initLinkedList();
        dataList = initLinkedList();
        entriesList = initLinkedList();

        processFile(*(argv+i), symbolsList, instructionsList, dataList, entriesList);
        i++;

        /* free allocated memory */
        freeList(symbolsList, freeSymbolContent);
        freeList(instructionsList, freeWordContent);
        freeList(dataList, freeDataContent);
        freeList(entriesList, freeEntryContent);
    }

    return 0;
}

/*
 * run assembler on a file
 * param baseFileName - base name of file to run the assembler on
 */
void processFile(const char *baseFileName, list_t *symbolsList, list_t *instructionsList, list_t *dataList, list_t *entriesList) {
    RESULT firstPassResult, secondPassResult;
    char *fullInputFileName;
    FILE *inputFile;

    fullInputFileName = malloc((strlen(baseFileName) + strlen(INPUT_FILE_SUFFIX) + 1) * sizeof(char));
    if(fullInputFileName == NULL){
        fprintf(stderr, "FATAL ERROR: Out of memory\n");
        exit(1);
    }

    strcat(fullInputFileName, baseFileName);
    strcat(fullInputFileName, INPUT_FILE_SUFFIX);

    printf("Opening input file: '%s'\n", fullInputFileName);

    inputFile = fopen(fullInputFileName, "r");

    if(inputFile == NULL) {
        fprintf(stderr, "Input file '%s' does not exist or cannot be opened!\n", fullInputFileName);
        return;
    }

    printf("Running first pass on input file: '%s'\n", fullInputFileName);
    firstPassResult = firstPass(inputFile, symbolsList, instructionsList, dataList, entriesList);

    printf("Running second pass on input file: '%s'\n", fullInputFileName);
    secondPassResult = secondPass(symbolsList, instructionsList, dataList, entriesList);

    printf("Generating output files for input file: '%s'\n", fullInputFileName);
    if(secondPassResult == SUCCESS && firstPassResult == SUCCESS)
        outputFiles(baseFileName, symbolsList, instructionsList, dataList, entriesList);

    fclose(inputFile);
    free(fullInputFileName);
    printf("Done processing input file: '%s'\n\n\n", fullInputFileName);
}
