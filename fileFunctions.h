#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "json.h"
#include "queue.h"

// Функции для работы с файлами.

bool addStringToFile(const char* fileName, const char* data);                          // Добавить строку в файл.
bool showContent(const char* fileName);                                                // Отобразить содержимое файла.
bool sortContent(const char* fileName, const char* field);                             // Сортировка содержимого файла по полю JSON объекта.
bool clearFile(const char* fileName);                                                  // Очистить файл.
bool readToQueue(QNODE** queue, const char* fileName);                                  // Записать данные из файла в очередь.
bool writeFromQueue(QNODE* queue, const char* fileName);                                // Записать данные из очереди в файл.

//----------------------------------------------------------------------------------------------------------------------

static bool ifFileEmpty(FILE* f);                                                      // Проверка, что файл пуст.
static int compareByField(const char* str1, const char* str2, const char* fieldName);  // Сравнение двух строк файла по указанному полю JSON объекта.
static void quicksort(char*** arr, int left, int right, const char* fieldName);        // Быстрая сортировка для строк файла.