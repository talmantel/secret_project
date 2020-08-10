#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "errors.h"
#include "linked_list.h"
#include "second_pass.h"
#include "symbols.h"
#include "words.h"
#include "entries.h"
#include "extern.h"

RESULT replaceLabelWithAddress(const char *fileName, list_t *symbolsList, list_t *externsList, word_t *word,
                               unsigned int instructionAddress, unsigned int wordAddress);
RESULT addAddressToEntry(const char *fileName, list_t *symbolsList, entry_t *entry);

/*this function is doing the second pass part of the file's parsing and assembling.
 * the function corrects and adds all the addresses.
 * returns ERROR if there was an error while parsing the file in the first pass, and SUCCESS otherwise
 * param fileName - the file name that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param wordList - a pointer to instruction list
 * param entriesList - a pointer to entries list
 * param externsList - a pointer to externs list*/
RESULT secondPass(const char *fileName, list_t *symbolsList, list_t *wordList, list_t *entriesList, list_t *externsList) {
    RESULT result = SUCCESS;
    word_t *word;
    entry_t *entry;
    node_t *currentNode;

    /* The address of the specific current label\instruction, in memory */
    unsigned int wordAddress = START_ADDRESS;

    /* The address of the instruction to which the current label\instruction belongs, in memory */
    unsigned int currentInstructionAddress;

    /* replace labels in instructions with symbol addresses */
    currentNode = wordList->head;
    while(currentNode != NULL){
        word = currentNode->content;
        switch (word->type) {
            case WORD_TYPE_LABEL:
                if(replaceLabelWithAddress(fileName, symbolsList, externsList, word, currentInstructionAddress,
                                           wordAddress) == ERROR)
                    result = ERROR;
                break;
            case WORD_TYPE_INSTRUCTION:
                /* update current instruction address, since we started a new instruction*/
                currentInstructionAddress = wordAddress;
                break;
            default:
                break;
        }

        currentNode = currentNode->next;
        wordAddress++;
    }


    /* add addresses to entries */
    currentNode = entriesList->head;
    while(currentNode != NULL){
        entry = currentNode->content;
        if(addAddressToEntry(fileName, symbolsList, entry) == ERROR)
            result = ERROR;

        currentNode = currentNode->next;
    }

    return result;
}

/*this function adds address to an entry record.
 *the function searches for a symbol with the name of the entry and sets the address accordingly.
 * returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the file name that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param entry - a pointer to the entry_t that the address should be added to*/
RESULT addAddressToEntry(const char *fileName, list_t *symbolsList, entry_t *entry) {
    /* find symbol with the same name */
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, entry->name);

    if(symbol == NULL){
        printErrorWithLine(fileName, entry->lineNumber, "'%s' is not defined\n", entry->name);
        return ERROR;
    }

    if(symbol->type == EXTERNAL){
        printErrorWithLine(fileName, entry->lineNumber,
                           "External label '%s' cannot be used as operand in 'entry' command\n", entry->name);
        return ERROR;
    }

    /* copy the symbol's address into entry */
    entry->address = symbol->address;
    return SUCCESS;
}

/*this function replaces all of the symbols labels with the respective addresses.
 * returns ERROR if there was any error and SUCCESS otherwise.
 * param fileName - the file name that is currently being parsed
 * param symbolsList - a pointer to symbols list
 * param externsList - a pointer to externs list
 * param word - a pointer to the current word_t we working on
 * param instructionAddress - the address of the current instruction
 * param wordAddress - the address of the current word*/
RESULT replaceLabelWithAddress(const char *fileName, list_t *symbolsList, list_t *externsList, word_t *word,
                               unsigned int instructionAddress, unsigned int wordAddress) {
    address_t *address;
    extern_t *external;

    /* find symbol with the same name as the label*/
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, word->content.label->labelName);

    if(symbol == NULL){
        printErrorWithLine(fileName, word->content.label->lineNumber, "'%s' is not defined\n",
                           word->content.label->labelName);
        return ERROR;
    }

    address = malloc(sizeof(address_t));
    if(address == NULL)
        handleAllocError();

    switch (word->content.label->addressing_type) {
        case ADDRESSING_TYPE_DIRECT:
            if(symbol->type == EXTERNAL){
                address->addressValue = 0;
                address->are_type = E;

                /* add to list of externals */
                external = malloc(sizeof(extern_t));
                if(external == NULL)
                    handleAllocError();

                external->name = malloc((strlen(symbol->name) + 1) * sizeof(char));
                if(external->name == NULL)
                    handleAllocError();
                strcpy(external->name, symbol->name);
                external->address = wordAddress;
                addNode(externsList, external);
            }
            else {
                address->addressValue = symbol->address;
                address->are_type = R;
            }
            break;
        case ADDRESSING_TYPE_RELATIVE:
            if(symbol->type == EXTERNAL){
                printErrorWithLine(fileName, word->content.label->lineNumber,
                                   "External label '%s' cannot be used as relative operand\n",
                                   word->content.label->labelName);
                free(address);
                return ERROR;
            }

            address->addressValue = symbol->address - instructionAddress;
            address->are_type = A;
            break;
        default:
            /* Should never happen, labels can only be direct or relative */
            break;
    }

    /* replace the data in the word - free the label, and store the address instead */
    freeLabelContent(word->content.label);
    word->type = WORD_TYPE_ADDRESS;
    word->content.address = address;
    return SUCCESS;
}


