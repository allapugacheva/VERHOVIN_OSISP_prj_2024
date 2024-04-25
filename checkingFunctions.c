#include "checkingFunctions.h"

pthread_t thread1;
pthread_t thread2;

struct snode* checkStack1 = NULL;
struct snode* checkStack2 = NULL;

bool* boolError;
int* minusOne;
int* zero;

// Функция проверки, что указать на указатель не пустой.
bool checkDoublePointer(const void** pointer) {

    return pointer != NULL && *pointer != NULL;
}

// Функция проверки, что число в заданных границах.
bool checkNumber(int number, int min, int max) {

    return number >= min && (max == -1 || number <= max);
}

// Функция проверки, что массив не содержит пустых элементов.
bool checkArray(const void** array, int n) {

    if(array == NULL)
        return false;

    for(int i = 0; i<n; i++)
        if(array[i] == NULL)
            return false;

    return true;
}

// Проверка строки на соответствие формата json.
bool checkJsonString(const char* jsonString) {

    pthread_self() == thread1 ? sPush(&checkStack1, "checkJsonString") : sPush(&checkStack2, "checkJsonString");

    if(jsonString == NULL)
        return *(bool*)handleError("jsonString is NULL.", true, NULL, 1, NULL, 0, NULL);

    int i = 0, iLen = stringLength(jsonString);
    SNODE* stack = NULL;

    while(i < iLen) {

        if ((jsonString[i] == '{' || jsonString[i] == '"' || jsonString[i] == '[' || jsonString[i] == ':') &&
            ((stack != NULL && stack->string[0] != '"') || stack == NULL))
            sPushC(&stack, jsonString[i]);
        else if ((jsonString[i] == ',' && (stack != NULL && stack->string[0] == ':'))
                 || (jsonString[i] == '"' && (stack != NULL && stack->string[0] == '"'))
                 || (jsonString[i] == '}' && (stack != NULL && stack->string[0] == '{'))
                 || (jsonString[i] == ']' && (stack != NULL && stack->string[0] == '[')))
            sPop(&stack);
        else if (jsonString[i] == '}' && (stack != NULL && stack->string[0] == ':')) {
            sPop(&stack);
            sPop(&stack);
        }

        i++;
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);

    if(stack == NULL)
        return true;
    while(stack != NULL)
        sPop(&stack);
    return false;
}

// Вывод стека в лог ошибок.
void showStack() {

    SNODE* temp = pthread_self() == thread1 ? checkStack1 : checkStack2;                                     // Выбор нужного потока.

    if(temp != NULL) {
        fprintf(stderr, "[");
        while (temp != NULL) {                                                                               // Обход стека.
            fprintf(stderr, "%s", temp->string);
            if(temp->next != NULL)
                fprintf(stderr, "->");
            temp = temp->next;
        }
        fprintf(stderr, "]: ");
    }
}

// Обработчик ошибок.
void* handleError(const char* message, bool popStack, bool* changeContinuing, int errorType, FILE* closeFile, int nSockets, int* closeSockets) {

    showStack();
    fprintf(stderr, "%s\n", message);                                                          // Запись сообщения.

    if(popStack == true)                                                                                     // Очистить стек, если надо.
        pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);

    if(changeContinuing != NULL)                                                                            // Установить статус ошибки.
        *changeContinuing = error;

    if(closeFile != NULL)                                                                                   // Закрыть файл.
        fclose(closeFile);

    for(int i = 0; i< nSockets; i++)                                                                        // Закрыть сокеты.
        close(closeSockets[i]);

    switch (errorType) {

        case 1:                                                                                             // Возврат нужного типа ошибки.
            return boolError;
        case 2:
            return JSON_ERROR;
        case 3:
            return HEADER_ERROR;
        case 4:
            return RESPONSE_ERROR;
        case 5:
            return REQUEST_ERROR;
        case 6:
            return minusOne;
        case 7:
            return zero;
        default:
            return NULL;
    }
}

void initNums() {

    boolError = (bool*)malloc(sizeof(bool));
    *boolError = error;

    minusOne = (int*)malloc(sizeof(int));
    *minusOne = -1;

    zero = (int*)malloc(sizeof(int));
    *zero = 0;
}

void freeNums() {

    free(boolError);
    free(minusOne);
    free(zero);
}