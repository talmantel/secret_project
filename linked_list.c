#include <stdlib.h>
#include "linked_list.h"
#include "errors.h"

/*a list initializer function.
 * the function initialize the list's properties and returns a pointer to the list*/
list_t * initLinkedList(){
    list_t * list = malloc(sizeof(list_t));
    if(list == NULL)
        handleAllocError();

    list->head = NULL;
    list->tail = NULL;
    list->length = 0;

    return list;
}

/*a function to add a node to the list.
 * param list - a pointer to the list that the node should be add to
 * param content - a pointer to the content of the node that should be added */
void addNode(list_t * list, void *content){
    node_t * newNode = malloc(sizeof(node_t));
    if(newNode == NULL)
        handleAllocError();

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

/* a search function in the list.
 * the function search if there is a node that we are looking for in the list.
 * if there is -> returns a pointer to that node. otherwise returns NULL
 * param list - a pointer to the list that the node should be add to
 * param equals - a pointer to a comparer function to be used to compare the nodes with the content we looking for.
 * param toFind - a pointer to the content we are looking for in the list*/
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

/* a function to free the allocated memory of the list.
 * param list - a pointer to the list that needs to be freed
 * param freeContent - a pointer to a function that frees the allocated memory of a node in the list*/
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
