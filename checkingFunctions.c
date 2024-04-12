#include "checkingFunctions.h"

void checkPointer(const void* pointer, const char* functionName, const char* pointerName) {

    if(pointer == NULL) {
        printf("Error in '%s' - NULL %s\n", functionName, pointerName);
        exit(0);
    }
}

void checkDoublePointer(void** pointer, const char* functionName, const char* pointerName) {

    if(pointer == NULL || *pointer == NULL) {
        printf("Error in '%s' - NULL %s\n", functionName, pointerName);
        exit(0);
    }
}

void checkNumber(int size, int min, int max, const char* functionName) {

    if(size < min || (max != -1 && size > max)) {
        printf("Error in '%s' - wrong number\n", functionName);
        exit(0);
    }
}