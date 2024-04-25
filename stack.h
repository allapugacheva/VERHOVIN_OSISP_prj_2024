#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "stringFunctions.h"

typedef struct snode {
    struct snode* next;
    struct snode* prev;
    char* string;
} SNODE;

void sPushC(SNODE** stack, char c);
void sPush(SNODE** stack, const char* string);
void sPop(SNODE** stack);

//----------------------------------------------------------------------------------------------------------------------

static SNODE* makeSnodeC(char c);
static SNODE* makeSnode(const char* string);