#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "checkingFunctions.h"

// Библиотека для работы со строками.

int stringLength(const char* string);                                     // Поиск длины строки.
void copyString(char** to, const char* from);                             // Копирование строки.
int compareString(const char* str1, const char* str2);                    // Сравнение строк.
void addString(char** to, const char* from);                              // Конкатенация строк.
char* copyStringFromIToSymbol(const char* from, int* i, char to);         // Копирование строки с выбранного места.
int findNumberLen(int number);                                            // Поиск длины числа при представлении его в строку.
bool isNum(char c);                                                       // Проверка на цифру и знак минуса.
double stringToDouble(const char* str);                                   // Конвертация строки в дробное число.
int divisorForDouble(int number);                                         // Делитель для дробного числа при преобразовании его из строки.
int findSubstring(const char* string, const char* substring);             // Поиск первого вхождения подстроки.