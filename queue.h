#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "stringFunctions.h"

typedef struct node {
    struct node* next;
    struct node* prev;
    char* data;
} NODE;

void push(NODE** queue, const char* data);
void pop(NODE** queue);