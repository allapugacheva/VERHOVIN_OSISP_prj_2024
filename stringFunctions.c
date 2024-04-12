#include "stringFunctions.h"

int stringLength(const char* string) {

    int length = 0;
    if(string != NULL)
        for(; string[length] != '\0'; length++);

    return length;
}

void copyString(char** to, const char* from) {

    if (to != NULL && from != NULL) {
        *to = (char *) malloc(stringLength(from) + 1);
        int iLen = stringLength(from);

        for (int i = 0; i <= iLen; i++)
            (*to)[i] = from[i];
    }
}

int compareString(const char* str1, const char* str2) {

    int len1 = stringLength(str1), len2 = stringLength(str2),
            minimal = len1 < len2 ? len1 : len2;

    for(int i = 0; i<minimal; i++)
        if(str1[i] != str2[i])
            return str1[i] - str2[i];

    if(len1 == len2)
        return 0;
    else if(len1 < len2)
        return -1;
    else
        return 1;
}

void addString(char** to, const char* from) {

    if(to != NULL && from != NULL) {
        int i = stringLength(*to), jLen = stringLength(from);
        *to = (char *) realloc(*to, (i + jLen + 1) * sizeof(char));

        for (int j = 0; j < jLen; j++, i++)
            (*to)[i] = from[j];

        (*to)[i] = '\0';
    }
}

char* copyStringFromIToSymbol(const char* from, int* i, char to) {

    if(from != NULL && i != NULL) {
        int j, a;

        for (j = *i; from[j] != to && from[j] != '\0'; j++);
        char *temp = (char *) malloc(j - *i + 1);
        for (a = 0; *i < j; (*i)++, a++)
            temp[a] = from[*i];
        temp[a] = '\0';

        return temp;
    }
    return NULL;
}

int findNumberLen(int number) {

    int i = 0;
    while (number != 0) {
        number /= 10;
        i++;
    }

    return i;
}

bool isNum(char c) {
    return ((c >= '0' && c <= '9') || c == '-');
}

double CTOD(const char* str) {

    double num = 0;
    int i = 0, divisor = 1;

    while(i < stringLength(str) && !isNum(str[i]))
        i++;

    bool point = false, negative = (str[i] == '-');

    for(; i < stringLength(str) && (isNum(str[i]) || str[i] == '.'); i++) {

        if(str[i] == '.') {
            point = true;
            continue;
        }

        num = num * 10 + str[i] - '0';

        if(point)
            divisor *= 10;
    }

    num /= divisor;
    if(negative)
        num *= -1;

    return num;
}
int divisorForDouble(int number) {

    int divisor = 1;
    while(number != 0) {
        divisor *= 10;
        number /= 10;
    }

    return divisor;
}
int findSubstring(const char* string, const char* substring) {

    int i = 0, j = 0, iLen = stringLength(string), jLen = stringLength(substring), tempI = i;

    while(true) {
        i = tempI;
        j = 0;

        while (i < iLen && string[i++] != substring[j]);

        if (i == iLen)
            return -1;
        tempI = i;

        while (i < iLen && j < jLen && string[i++] == substring[j++]);

        if(j == jLen)
            return tempI;
        tempI++;
    }
}