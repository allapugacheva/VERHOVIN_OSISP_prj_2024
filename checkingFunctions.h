#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "bool.h"
#include "stack.h"
#include "stringFunctions.h"
#include "json.h"
#include "http.h"

struct snode;

extern pthread_t thread1;
extern pthread_t thread2;

extern struct snode* checkStack1;
extern struct snode* checkStack2;

extern bool* boolError;
extern int* minusOne;
extern int* zero;

bool checkDoublePointer(const void** pointer);         // Проверить указатель на указатель.
bool checkNumber(int number, int min, int max);        // Проверить номер.
bool checkArray(const void** array, int n);            // Проверить массив на наличие пустых элементов.
bool checkJsonString(const char* jsonString);          // Проверить строку на соответствие формату json.

void showStack();                                      // Вывод стека вызовов в лог.
void* handleError(const char* message, bool popStack, bool* changeContinuing, int errorType, FILE* closeFile, int nSockets, int* closeSockets); // Обработчик ошибок.

void initNums();
void freeNums();