#include <stdlib.h>
#include <string.h>
#include "definitions.h"
#include "errors.h"
#include "linked_list.h"
#include "second_pass.h"
#include "symbols.h"
#include "instructions.h"
#include "entries.h"
#include "externals.h"

RESULT replaceLabelWithAddress(const char *fileName, list_t *symbolsList, list_t *externalsList, word_t *word,
                               unsigned int instructionAddress, unsigned int wordAddress);

RESULT addAddressToEntry(const char *fileName, list_t *symbolsList, entry_t *entry);

RESULT secondPass(const char *fileName, list_t *symbolsList, list_t *instructionsList, list_t *entriesList,
                  list_t *externalsList) {
    RESULT result = SUCCESS;
    word_t *word;
    entry_t *entry;
    node_t *currentNode;

    /* The address of the specific current label\instruction, in memory */
    unsigned int wordAddress = START_ADDRESS;

    /* The address of the instruction to which the current label\instruction belongs, in memory */
    unsigned int currentInstructionAddress;

    /* replace labels in instructions with symbol addresses */
    currentNode = instructionsList->head;
    while(currentNode != NULL){
        word = currentNode->content;
        switch (word->type) {
            case WORD_TYPE_LABEL:
                if(replaceLabelWithAddress(fileName, symbolsList, externalsList, word, currentInstructionAddress,
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


RESULT addAddressToEntry(const char *fileName, list_t *symbolsList, entry_t *entry) {
    /* find symbol with the same name */
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, entry->name);

    if(symbol == NULL){
        printError(fileName, entry->lineNumber, "'%s' is not defined\n", entry->name);
        return ERROR;
    }

    if(symbol->type == EXTERNAL){
        printError(fileName, entry->lineNumber, "External label '%s' cannot be used as operand in 'entry' command\n", entry->name);
        return ERROR;
    }

    /* copy the symbol's address into entry */
    entry->address = symbol->address;
    return SUCCESS;
}

RESULT replaceLabelWithAddress(const char *fileName, list_t *symbolsList, list_t *externalsList, word_t *word,
                               unsigned int instructionAddress, unsigned int wordAddress) {
    address_t *address;
    external_t *external;

    /* find symbol with the same name as the label*/
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, word->content.label->label);

    if(symbol == NULL){
        printError(fileName, word->content.label->lineNumber, "'%s' is not defined\n", word->content.label->label);
        return ERROR;
    }

    address = malloc(sizeof(address_t));
    if(address == NULL)
        handleError(ERROR_OUT_OF_MEMORY);

    switch (word->content.label->addressing_type) {
        case ADDRESSING_TYPE_DIRECT:
            if(symbol->type == EXTERNAL){
                address->address = 0;
                address->are_type = E;

                /* add to list of externals */
                external = malloc(sizeof(external_t));
                if(external == NULL)
                    handleError(ERROR_OUT_OF_MEMORY);

                external->name = malloc((strlen(symbol->name) + 1) * sizeof(char));
                if(external->name == NULL)
                    handleError(ERROR_OUT_OF_MEMORY);
                strcpy(external->name, symbol->name);
                external->address = wordAddress;
                addNode(externalsList, external);
            }
            else {
                address->address = symbol->address;
                address->are_type = R;
            }
            break;
        case ADDRESSING_TYPE_RELATIVE:
            if(symbol->type == EXTERNAL){
                printError(fileName, word->content.label->lineNumber, "External label '%s' cannot be used as relative operand\n", word->content.label->label);
                free(address);
                return ERROR;
            }

            address->address = symbol->address - instructionAddress;
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


