#pragma once
#include <stdio.h>
#include <stdlib.h>

void checkPointer(const void* pointer, const char* functionName, const char* pointerName);
void checkDoublePointer(void** pointer, const char* functionName, const char* pointerName);
void checkNumber(int size, int min, int max, const char* functionName);