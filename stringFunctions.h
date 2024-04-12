#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int stringLength(const char* string);
void copyString(char** to, const char* from);
int compareString(const char* str1, const char* str2);
void addString(char** to, const char* from);
char* copyStringFromIToSymbol(const char* from, int* i, char to);
int findNumberLen(int number);
bool isNum(char c);
double CTOD(const char* str);
int divisorForDouble(int number);
int findSubstring(const char* string, const char* substring);