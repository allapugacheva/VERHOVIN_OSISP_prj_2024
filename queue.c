#include "queue.h"

static NODE* makeNode(const char* data) {

    NODE* node = (NODE*)malloc(sizeof(NODE));

    node->next = NULL;
    node->prev = NULL;
    copyString(&node->data, data);

    return node;
}

void push(NODE** queue, const char* data) {

    if(queue != NULL) {
        if (*queue == NULL)
            *queue = makeNode(data);
        else {
            NODE* temp = *queue;
            while(temp->next != NULL)
                temp = temp->next;

            temp->next = makeNode(data);
            temp->next->prev = temp;
        }
    }
}

void pop(NODE** queue) {

    if(queue != NULL && *queue != NULL) {

        NODE* temp = *queue;
        *queue = (*queue)->next;

        free(temp);
        temp = NULL;
    }
}