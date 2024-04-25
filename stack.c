#include "stack.h"

// Создание узла с символом.
static SNODE* makeSnodeC(char c) {

    SNODE* temp = (SNODE*)malloc(sizeof(SNODE));

    temp->next = NULL;
    temp->prev = NULL;
    temp->string = (char*)malloc(2);
    temp->string[0] = c;
    temp->string[1] = '\0';

    return temp;
}

// Добавление узла с символом.
void sPushC(SNODE** stack, char c) {

    if (stack != NULL) {

        if (*stack != NULL) {

            (*stack)->prev = makeSnodeC(c);
            (*stack)->prev->next = *stack;
            *stack = (*stack)->prev;
        } else
            *stack = makeSnodeC(c);
    }
}

// Создание узла со строкой.
static SNODE* makeSnode(const char* string) {

    SNODE* temp = (SNODE*)malloc(sizeof(SNODE));
    temp->next = NULL;
    temp->prev = NULL;

    int len = 0;
    for(;string[len] != '\0'; len++);
    temp->string = (char*)malloc(len + 1);
    for(int i = 0; i<= len; i++)
        temp->string[i] = string[i];

    return temp;
}

// Добавление узла со строкой.
void sPush(SNODE** stack, const char* string) {

    if (stack != NULL) {

        if (*stack != NULL) {

            (*stack)->prev = makeSnode(string);
            (*stack)->prev->next = *stack;
            *stack = (*stack)->prev;
        } else {
            *stack = makeSnode(string);
        }
    }
}

// Удаление элемента.
void sPop(SNODE** stack) {

    if(stack != NULL && *stack != NULL) {

        SNODE* temp = *stack;
        *stack = (*stack)->next;
        free(temp);
        temp = NULL;
    }

}