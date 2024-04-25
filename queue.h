#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "stringFunctions.h"

// Очередь.

typedef struct qnode {
    struct qnode* next;
    struct qnode* prev;
    char* data;
} QNODE;

void qPush(QNODE** queue, const char* data);  // Добавление в очередь.
void qPop(QNODE** queue);                     // Удаление из очереди.

// ---------------------------------------------------------------------------------------------------------------------

static QNODE* makeQnode(const char* data);    // Создание нового узла очереди.