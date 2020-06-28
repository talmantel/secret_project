#include <stdio.h>
#include <string.h>

#include "definitions.h"
#include "linked_list.h"
#include "first_pass.h"
#include "second_pass.h"
#include "output.h"

void processFile(char *baseFileName, LIST symbolsList, LIST instructionsList, LIST dataList, LIST entriesList);

int main(int argc, char** argv) {

    LIST symbolsList, instructionsList, dataList, entriesList;

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
        freeList(symbolsList);
        freeList(instructionsList);
        freeList(dataList);
        freeList(entriesList);
    }

    return 0;
}

/*
 * run assembler on a file
 * param baseFileName - base name of file to run the assembler on
 */
void processFile(char *baseFileName, LIST symbolsList, LIST instructionsList, LIST dataList, LIST entriesList) {
    RESULT firstPassResult, secondPassResult;
    char *fullInputFileName;
    FILE *inputFile;

    fullInputFileName = strcat(baseFileName, INPUT_FILE_SUFFIX);

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
    printf("Done processing input file: '%s'\n\n\n", fullInputFileName);
}
