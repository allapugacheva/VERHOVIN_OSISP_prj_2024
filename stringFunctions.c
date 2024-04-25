#include "stringFunctions.h"

// Поиск длины строки.
int stringLength(const char* string) {

    pthread_self() == thread1 ? sPush(&checkStack1, "stringLength") : sPush(&checkStack2, "stringLength");

    if(string == NULL)
        handleError("string is NULL.", false, NULL, 0, NULL, 0, NULL);

    int length = 0;
    if(string != NULL)
        for(; string[length] != '\0'; length++);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return length;
}

// Копирование строки.
void copyString(char** to, const char* from) {

    pthread_self() == thread1 ? sPush(&checkStack1, "copyString") : sPush(&checkStack2, "copyString");

    if (to != NULL && from != NULL) {
        *to = (char *) malloc(stringLength(from) + 1);
        int iLen = stringLength(from);

        for (int i = 0; i <= iLen; i++)
            (*to)[i] = from[i];
    } else
        handleError("to or from is NULL.", false, NULL, 0, NULL, 0, NULL);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
}

// Сравнение строк.
int compareString(const char* str1, const char* str2) {

    int len1 = stringLength(str1), len2 = stringLength(str2), minimal = len1 < len2 ? len1 : len2;

    if (len1 == len2)
        for (int i = 0; i < minimal; i++)
            if (str1[i] != str2[i])
                return str1[i] - str2[i];

    return len1 == len2 ? 0 : (len1 < len2 ? -1 : 1);
}

// Конкатенация строк.
void addString(char** to, const char* from) {

    pthread_self() == thread1 ? sPush(&checkStack1, "addString") : sPush(&checkStack2, "addString");

    if(to != NULL && from != NULL) {
        int i = stringLength(*to), jLen = stringLength(from);
        *to = (char *) realloc(*to, (i + jLen + 1) * sizeof(char));

        for (int j = 0; j < jLen; j++, i++)
            (*to)[i] = from[j];

        (*to)[i] = '\0';
    } else
        handleError("to or from is NULL.", false, NULL, 0, NULL, 0, NULL);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
}

// Копирование строки с заданной позиции до указанного символа.
char* copyStringFromIToSymbol(const char* from, int* i, char to) {

    pthread_self() == thread1 ? sPush(&checkStack1, "copyStringFromIToSymbol") : sPush(&checkStack2, "copyStringFromIToSymbol");

    if(from != NULL && i != NULL) {
        int j, a;

        for (j = *i; from[j] != to && from[j] != '\0'; j++);
        char *temp = (char *) malloc(j - *i + 1);
        for (a = 0; *i < j; (*i)++, a++)
            temp[a] = from[*i];
        temp[a] = '\0';

        pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
        return temp;
    }

    return handleError("from or i is NULL.", true, NULL, 0, NULL, 0, NULL);
}

// Поиск длины числа при его представлении в виде строки.
int findNumberLen(int number) {

    int i = 0;
    while (number != 0) {
        number /= 10;
        i++;
    }

    return i;
}

// Проверка символа на соответствие цифре или знаку минус.
bool isNum(char c) {
    return ((c >= '0' && c <= '9') || c == '-');
}

// Конвертация строки в дробное число.
double stringToDouble(const char* str) {

    pthread_self() == thread1 ? sPush(&checkStack1, "stringToDouble") : sPush(&checkStack2, "stringToDouble");

    if(str == NULL)
        return *(int*)handleError("str is NULL.", true, NULL, 7, NULL, 0, NULL);

    double num = 0;
    int i = 0, divisor = 1, iLen = stringLength(str);

    while(i < iLen && isNum(str[i]) == false)
        i++;

    bool point = false, negative = (str[i] == '-');

    for(; i < iLen && (isNum(str[i]) || str[i] == '.'); i++) {

        if(str[i] == '.') {
            point = true;
            continue;
        }

        num = num * 10 + str[i] - '0';

        if(point == true)
            divisor *= 10;
    }

    num /= divisor;
    if(negative == true)
        num *= -1;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return num;
}

// Поиск делителя для дробного числа.
int divisorForDouble(int number) {

    int divisor = 1;
    while(number != 0) {
        divisor *= 10;
        number /= 10;
    }

    return divisor;
}

// Поиск первого вхождения подстроки в строку.
int findSubstring(const char* string, const char* substring) {

    pthread_self() == thread1 ? sPush(&checkStack1, "findSubstring") : sPush(&checkStack2, "findSubstring");

    if(string == NULL || substring == NULL)
        return *(int*) handleError("string or substring is NULL.", true, NULL, 6, NULL, 0, NULL);

    int i = 0, j, iLen = stringLength(string), jLen = stringLength(substring), tempI = i;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);

    while(true) {
        i = tempI;
        j = 0;

        while (i < iLen && string[i++] != substring[j]);

        if (i == iLen)
            return -1;
        tempI = i;
        j++;

        while (i < iLen && j < jLen && string[i++] == substring[j++]);

        if(j == jLen)
            return tempI;
        tempI++;
    }
}