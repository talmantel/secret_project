#ifndef MMN14_LINKED_LIST_H
#define MMN14_LINKED_LIST_H

struct {
    void *content;
    struct sNODE *next;
} sNODE;

typedef struct sNODE *NODE;
typedef struct {
    int length;
    NODE head;
    NODE tail;
} *LIST;

LIST initLinkedList();
void addNode(LIST list, void *content);
void *search(LIST list, BOOL (*compare)(void *find, void *content), void *toFind);
void freeList(LIST list);

#endif //MMN14_LINKED_LIST_H
