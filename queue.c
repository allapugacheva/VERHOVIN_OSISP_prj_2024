#include "queue.h"

// Создание узла очереди.
static QNODE* makeQnode(const char* data) {

    QNODE* node = (QNODE*)malloc(sizeof(QNODE));

    node->next = NULL;
    node->prev = NULL;
    copyString(&node->data, data);

    return node;
}

// Добавление в очередь.
void qPush(QNODE** queue, const char* data) {

    if(queue != NULL && data != NULL) {
        if (*queue == NULL)
            *queue = makeQnode(data);
        else {
            QNODE* temp = *queue;
            while(temp->next != NULL)
                temp = temp->next;

            temp->next = makeQnode(data);
            temp->next->prev = temp;
        }
    }
}

// Удаление из очереди.
void qPop(QNODE** queue) {

    if(queue != NULL && *queue != NULL) {

        QNODE* temp = *queue;
        *queue = (*queue)->next;

        free(temp);
        temp = NULL;
    }
}