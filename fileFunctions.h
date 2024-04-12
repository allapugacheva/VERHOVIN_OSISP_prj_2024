#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <fcntl.h>
#include "stringFunctions.h"
#include "json.h"
#include "queue.h"

FILE* openFile(const char* fileName, const char* mode);
void addStringToFile(const char* fileName, const char* data);
void showContent(const char* fileName);
void sortContent(const char* fileName, const char* field);
void clearFile(const char* fileName);
void readToQueue(NODE** queue, const char* fileName);
void writeFromQueue(NODE* queue, const char* fileName);
void sortFileByJsonField(const char* fileName, const char* fieldName);