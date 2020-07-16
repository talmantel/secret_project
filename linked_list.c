#include <stdlib.h>
#include "linked_list.h"
#include "errors.h"

list_t * initLinkedList(){
    list_t * list = malloc(sizeof(list_t));
    if(list == NULL)
        handleError(ERROR_OUT_OF_MEMORY);

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}

void addNode(list_t * list, void *content){
    node_t * newNode = malloc(sizeof(node_t));
    if(newNode == NULL)
        handleError(ERROR_OUT_OF_MEMORY);

    newNode->content = content;
    newNode->next =NULL;

    if (list->length == 0){
        list->head = newNode;
        list->tail = newNode;
    } else {
        list->tail->next = newNode;
        list->tail = newNode;
    }
    list->length++;
}

void * search(list_t * list, int (*equals)(void * find, void * content), void *toFind){
    node_t * current = list->head;
    while (current != NULL){
        if (equals(toFind, current->content)){
            return current->content;
        }
        current = current->next;
    }
    return NULL;
}

void freeList(list_t * list, void (*freeContent)(void *content)){
    node_t * current = list->head;
    node_t * temp;
    while (current != NULL){
        temp = current;
        current = temp->next;
        freeContent(temp->content);
        free(temp);
    }
    free(list);
}
