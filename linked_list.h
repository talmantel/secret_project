#ifndef MMN14_LINKED_LIST_H
#define MMN14_LINKED_LIST_H

/* a struct that represent a generic node in the list*/
typedef struct node{
    void *content;
    struct node * next;
} node_t;

/* a struct that represent a list*/
typedef struct list{
    int length;
    node_t * head;
    node_t * tail;
} list_t;

/*a list initializer function.
 * the function initialize the list's properties and returns a pointer to the list*/
list_t * initLinkedList();

/*a function to add a node to the list.
 * param list - a pointer to the list that the node should be add to
 * param content - a pointer to the content of the node that should be added */
void addNode(list_t * list, void *content);

/* a search function in the list.
 * the function search if there is a node that we are looking for in the list.
 * if there is -> returns a pointer to that node. otherwise returns NULL
 * param list - a pointer to the list that the node should be add to
 * param equals - a pointer to a comparer function to be used to compare the nodes with the content we looking for.
 * param toFind - a pointer to the content we are looking for in the list*/
void *search(list_t * list, int (*equals)(void *find, void *content), void *toFind);

/* a function to free the allocated memory of the list.
 * param list - a pointer to the list that needs to be freed
 * param freeContent - a pointer to a function that frees the allocated memory of a node in the list*/
void freeList(list_t * list, void (*freeContent)(void *content));

#endif
