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

list_t * initLinkedList();
void addNode(list_t * list, void *content);
void *search(list_t * list, int (*equals)(void *find, void *content), void *toFind);
void freeList(list_t * list, void (*freeContent)(void *content));

#endif
