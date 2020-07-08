#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "definitions.h"
#include "errors.h"
#include "linked_list.h"
#include "second_pass.h"
#include "symbols.h"
#include "instructions.h"
#include "entries.h"
#include "externals.h"

RESULT handleLabel(list_t *symbolsList, list_t *externalsList, word_t *word, unsigned int instructionAddress, unsigned int wordAddress);

RESULT addAddressToEntry(list_t *symbolsList, entry_t *entry);

RESULT secondPass(list_t *symbolsList, list_t *instructionsList, list_t *entriesList, list_t *externalsList){
    RESULT result = SUCCESS;
    word_t *word;
    entry_t *entry;
    node_t *currentNode;

    unsigned int wordAddress = START_ADDRESS;
    unsigned int currentInstructionAddress;

    /* replace labels in instructions with symbol addresses */
    currentNode = instructionsList->head;
    while(currentNode != NULL){
        word = currentNode->content;
        switch (word->type) {
            case WORD_TYPE_LABEL:
                if(handleLabel(symbolsList,externalsList, word, currentInstructionAddress, wordAddress) == ERROR)
                    result = ERROR;
                break;
            case WORD_TYPE_INSTRUCTION:
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
        if(addAddressToEntry(symbolsList, entry) == ERROR)
            result = ERROR;

        currentNode = currentNode->next;
    }

    return result;
}

RESULT addAddressToEntry(list_t *symbolsList, entry_t *entry) {
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, entry->name);
    if(symbol == NULL){
        fprintf(stderr, "Error in line %d: '%s' is not defined\n", entry->lineNumber, entry->name);
        return ERROR;
    }

    if(symbol->type == EXTERNAL){
        fprintf(stderr, "Error in line %d: External label '%s' cannot be used as operand in 'entry' command\n", entry->lineNumber, entry->name);
        return ERROR;
    }

    entry->address = symbol->address;
    return SUCCESS;
}


RESULT handleLabel(list_t *symbolsList, list_t *externalsList, word_t *word, unsigned int instructionAddress, unsigned int wordAddress) {
    address_t *address;
    external_t *external;
    symbol_t *symbol = search(symbolsList, (int (*)(void *, void *)) compareSymbol, word->content.label->label);
    if(symbol == NULL){
        fprintf(stderr, "Error in line %d: '%s' is not defined\n", word->content.label->lineNumber, word->content.label->label);
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
                fprintf(stderr, "Error in line %d: External label '%s' cannot be used as relative operand\n",  word->content.label->lineNumber, word->content.label->label);
                free(address);
                return ERROR;
            }
            /* TODO check conversion issues, and completion to 2 to get negative values */
            address->address = symbol->address - instructionAddress;
            address->are_type = A;
            break;
        default:
            /* Should never happen, labels can only be direct or relative */
            break;
    }

    freeLabelContent(word->content.label);
    word->type = WORD_TYPE_ADDRESS;
    word->content.address = address;
    return SUCCESS;
}


