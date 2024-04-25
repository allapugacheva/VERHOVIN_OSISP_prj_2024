#include "json.h"

// Инициализация указателей объекта JSON пустыми указателями.
static void makeNullFields(JSON_NODE** node) {

    (*node)->current = NULL;
    (*node)->next = NULL;
    (*node)->objectFieldName = NULL;
    (*node)->prev = NULL;
    (*node)->stringVal = NULL;
}

// Очистка указателей объекта JSON.
static void freeNodePointers(JSON_NODE** jsonNode) {

    if((*jsonNode)->type == string)
        free((*jsonNode)->stringVal);

    if((*jsonNode)->type == array || (*jsonNode)->type == object)
        removeJson(&(*jsonNode)->current);                                    // Удаление всех связанных узлов массива или объекта.
}

// Изменение имени поля узла объекта JSON.
static void changeFieldName(JSON_NODE** jsonNode, const char* fieldName) {

    if(fieldName != NULL) {

        if((*jsonNode)->objectFieldName != NULL)
            free((*jsonNode)->objectFieldName);
        copyString(&(*jsonNode)->objectFieldName, fieldName);
    }
}

// Создание пустого объекта JSON.
JSON_NODE* createJsonNode() {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->type = null;

    return newNode;
}

// Создание объекта JSON с логической переменной.
JSON_NODE* createBoolJsonNode(bool boolValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createBoolJsonNode") : sPush(&checkStack2, "createBoolJsonNode");

    if(fieldName == NULL)
        return (JSON_NODE*)handleError("fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* newNode = JSON_MALLOC(1);                                            // Выделение места под объект JSON.
    makeNullFields(&newNode);                                                 // Инициализация указателей значениями NULL.

    newNode->boolVal = boolValue;                                                   // Указание типа и значения.
    newNode->type = boolean;

    if(fieldName[0] != '\0')                                                        // Указание имени поля, если задано.
        copyString(&newNode->objectFieldName, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return newNode;
}

// Создание объекта JSON с числом.
JSON_NODE* createNumberJsonNode(double numberValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createNumberJsonNode") : sPush(&checkStack2, "createNumberJsonNode");

    if(fieldName == NULL)
        return (JSON_NODE*)handleError("fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->numberVal = numberValue;
    newNode->type = number;

    if(fieldName[0] != '\0')
        copyString(&newNode->objectFieldName, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return newNode;
}

// Создание объекта JSON со строкой.
JSON_NODE* createStringJsonNode(const char* stringValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createStringJsonNode") : sPush(&checkStack2, "createStringJsonNode");

    if(stringValue == NULL || fieldName == NULL)
        return (JSON_NODE*)handleError("stringValue or fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);
    copyString(&(newNode->stringVal), stringValue);
    newNode->type = string;

    if(fieldName[0] != '\0')
        copyString(&newNode->objectFieldName, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return newNode;
}

// Создание объекта JSON с объектом.
JSON_NODE* createObjectJsonNode(JSON_NODE* objectValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createObjectJsonNode") : sPush(&checkStack2, "createObjectJsonNode");

    if(objectValue == NULL || fieldName == NULL)
        return (JSON_NODE*)handleError("objectValue or fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = objectValue;
    newNode->type = object;

    if(fieldName[0] != '\0')
        copyString(&newNode->objectFieldName, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return newNode;
}

// Создание объекта JSON c массивом.
JSON_NODE* createArrayJsonNode(JSON_NODE* arrayValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createArrayJsonNode") : sPush(&checkStack2, "createArrayJsonNode");

    if(arrayValue == NULL || fieldName == NULL)
        return (JSON_NODE*)handleError("arrayValue or fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = arrayValue;
    newNode->type = array;

    if(fieldName[0] != '\0')
        copyString(&newNode->objectFieldName, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return newNode;
}

// Удаление указанного узла из объекта JSON.
bool removeJsonNode(JSON_NODE** jsonNode) {

    pthread_self() == thread1 ? sPush(&checkStack1, "removeJsonNode") : sPush(&checkStack2, "removeJsonNode");

    if(checkDoublePointer((const void**)jsonNode) == false)
        return *(bool*)handleError("jsonNode is NULL.", true, NULL, 1, NULL, 0, NULL);

    bool moveHead = false;

    if((*jsonNode)->next != NULL)                           // Отсоединение указателей на следующий и предыдущий элементы.
        (*jsonNode)->next->prev = (*jsonNode)->prev;
    if((*jsonNode)->prev != NULL)
        (*jsonNode)->prev->next = (*jsonNode)->next;
    else 
        moveHead = true;

    freeNodePointers(jsonNode);                             // Освобождение указателей.

    if((*jsonNode)->objectFieldName != NULL)
        free((*jsonNode)->objectFieldName);

    if(moveHead == true) {

        JSON_NODE* temp = *jsonNode;                        // Очистка элемента и сдвиг указателя на первый элемент.
        *jsonNode = (*jsonNode)->next;

        free(temp);
    } else {

        free(*jsonNode);
        *jsonNode = NULL;
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Удаление всего объекта JSON.
bool removeJson(JSON_NODE** json) {

    pthread_self() == thread1 ? sPush(&checkStack1, "removeJson") : sPush(&checkStack2, "removeJson");

    if(json == NULL)
        return *(bool*)handleError("json is NULL.", true, NULL, 1, NULL, 0, NULL);

    while (removeJsonNode(json) == true);          // Очистка каждого узла.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Связь двух узлов объекта JSON.
static void connectNode(JSON_NODE** jsonNode, JSON_NODE** prevNode, JSON_NODE** nextNode) {

    if(nextNode != NULL)                                    // Установка указателя на следующий.
        (*jsonNode)->next = *nextNode;
    else
        (*jsonNode)->next = NULL;
    if(nextNode != NULL && *nextNode != NULL)               // Связь с предыдущим.
        (*nextNode)->prev = *jsonNode;

    if(prevNode != NULL)                                    // Установка указателя на предыдущий.
        (*jsonNode)->prev = *prevNode;
    else
        (*jsonNode)->prev = NULL;
    if(prevNode != NULL && *prevNode != NULL)               // Связь со следующим.
        (*prevNode)->next = *jsonNode;
}

// Количество узлов в объекте JSON.
static int jsonLength(JSON_NODE* json) {

    int length = 0;                                         // Обход всех узлов.
    for(JSON_NODE* temp = json; temp != NULL; temp = temp->next, length++);

    return length;
}

// Удаление узла на указанной позиции.
bool removeNJsonNode(JSON_NODE** json, int n) {

    pthread_self() == thread1 ? sPush(&checkStack1, "removeNJsonNode") : sPush(&checkStack2, "removeNJsonNode");

    if(json == NULL)
        return *(bool*) handleError("json is NULL.", true, NULL, 1, NULL, 0, NULL);

    JSON_NODE *temp = goToNJsonNode(*json, n);              // Переход к узлу.
    if(temp != NULL && IS_JSON_ERROR(*temp))
        return *(bool*) handleError("temp is NULL.", true, NULL, 1, NULL, 0, NULL);

    int result = removeJsonNode(&temp);            // Удаление.
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Удаление узла по указанному полю.
bool removeJsonNodeByField(JSON_NODE** json, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "removeJsonNodeByField") : sPush(&checkStack2, "removeJsonNodeByField");

    if(json == NULL)
        return *(bool*) handleError("json is NULL.", true, NULL, 1, NULL, 0, NULL);

    JSON_NODE* temp = goToJsonNodeByField(*json, fieldName); // Переход к узлу.
    if(temp != NULL && IS_JSON_ERROR(*temp))
        return *(bool*) handleError("temp is NULL.", true, NULL, 1, NULL, 0, NULL);

    int result = removeJsonNode(&temp);           // Удаление.
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Проверка, что в объекте JSON есть указанное поле.
bool hasJsonField(JSON_NODE* json, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "hasJsonField") : sPush(&checkStack2, "hasJsonField");

    if(json == NULL || fieldName == NULL)
        return *(bool*) handleError("json or fieldName is NULL.", true, NULL, 1, NULL, 0, NULL);

    JSON_NODE* temp = json;
    while(temp != NULL) {                                 // Обход всех узлов.
                                                          // Сравнение.
        if (compareString(temp->objectFieldName, fieldName) == 0) {

            pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
            return true;
        }
        temp = temp->next;
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return false;
}

// Переход к указанному полю.
JSON_NODE* goToJsonNodeByField(JSON_NODE* json, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "goToJsonNodeByField") : sPush(&checkStack2, "goToJsonNodeByField");

    if(json == NULL || fieldName == NULL)
        return (JSON_NODE*)handleError("json or fieldName is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* temp = json;                                  // Обход всех узлов и сравнение в каждом названии полей.
    for(;temp != NULL && compareString(temp->objectFieldName, fieldName) != 0; temp = temp->next);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return temp;
}

// Переход к указанному по счёту узлу.
JSON_NODE* goToNJsonNode(JSON_NODE* json, int n) {

    pthread_self() == thread1 ? sPush(&checkStack1, "goToNJsonNode") : sPush(&checkStack2, "goToNJsonNode");

    if(json == NULL || checkNumber(n, -1, jsonLength(json)) == false)
        return (JSON_NODE*)handleError("json is NULL or n has wrong value.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* temp = json;

    int count = 1;
    while(temp->next != NULL && count++ != n)                 // Переход к указанному по счёту полю.
        temp = temp->next;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return temp;
}

// Добавление узла на указанную позицию.
bool addToJsonToNPos(JSON_NODE** json, JSON_NODE* node, int position) {

    pthread_self() == thread1 ? sPush(&checkStack1, "addToJsonToNPos") : sPush(&checkStack2, "addToJsonToNPos");

    if (json == NULL || node == NULL || checkNumber(position, -1, *json != NULL ? jsonLength(*json) : 2) == false)
        return *(bool*) handleError("json or node is NULL or position has wrong value.", true, NULL, 1, NULL, 0, NULL);

    if (*json == NULL)                                        // Если объект пуст - в голову.
        *json = node;
    else {
        JSON_NODE *nNode = goToNJsonNode(*json, position); // Переход на нужное место.

        if (position == -1)                                   // Установка соединений.
            connectNode(&node, &nNode, NULL);            // В конец.
        else
            connectNode(&node, &nNode->prev, &nNode);    // Не в конец.

        if (position == 1)
            *json = (*json)->prev;                            // Сдвиг головы при добавлении в начало.
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения названия поля объекта JSON.
bool updateJsonNodeFieldName(JSON_NODE** jsonNode, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeFieldName") : sPush(&checkStack2, "updateJsonNodeFieldName");

    if(checkDoublePointer((const void**)jsonNode) == false || fieldName == NULL)
        return *(bool*)handleError("jsonNode or fieldName is NULL.", true, NULL, 1, NULL, 0, NULL);

    changeFieldName(jsonNode, fieldName);                    // Обновление названия поля.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения поля на логическую переменную.
bool updateJsonNodeToBool(JSON_NODE** jsonNode, bool boolValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeToBool") : sPush(&checkStack2, "updateJsonNodeToBool");

    if(checkDoublePointer((const void**)jsonNode) == false)
        return *(bool*) handleError("jsonNode is NULL.", true, NULL, 1, NULL, 0, NULL);

    freeNodePointers(jsonNode);

    (*jsonNode)->type = boolean;                             // Установка новых значений и типа и названия, если последнее задано.
    (*jsonNode)->boolVal = boolValue;
    changeFieldName(jsonNode, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения поля число.
bool updateJsonNodeToNumber(JSON_NODE** jsonNode, double numberValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeToNumber") : sPush(&checkStack2, "updateJsonNodeToNumber");

    if(checkDoublePointer((const void**)jsonNode) == false)
        return *(bool*) handleError("jsonNode is NULL.", true, NULL, 1, NULL, 0, NULL);

    freeNodePointers(jsonNode);

    (*jsonNode)->type = number;
    (*jsonNode)->numberVal = numberValue;
    changeFieldName(jsonNode, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения поля на строку.
bool updateJsonNodeToString(JSON_NODE** jsonNode, const char* stringValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeToString") : sPush(&checkStack2, "updateJsonNodeToString");

    if(checkDoublePointer((const void**)jsonNode) == false || stringValue == NULL)
        return *(bool*) handleError("jsonNode or stringValue is NULL.", true, NULL, 1, NULL, 0, NULL);

    freeNodePointers(jsonNode);

    (*jsonNode)->type = string;
    copyString(&(*jsonNode)->stringVal, stringValue);
    changeFieldName(jsonNode, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения поля на объект.
bool updateJsonNodeToObject(JSON_NODE** jsonNode, JSON_NODE* objectValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeToObject") : sPush(&checkStack2, "updateJsonNodeToObject");

    if(checkDoublePointer((const void**)jsonNode) == false || objectValue == NULL)
        return *(bool*) handleError("jsonNode or objectValue is NULL.", true, NULL, 1, NULL, 0, NULL);

    freeNodePointers(jsonNode);

    (*jsonNode)->type = object;
    (*jsonNode)->current = objectValue;
    changeFieldName(jsonNode, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Обновление значения поля на массив.
bool updateJsonNodeToArray(JSON_NODE** jsonNode, JSON_NODE* arrayValue, const char* fieldName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "updateJsonNodeToArray") : sPush(&checkStack2, "updateJsonNodeToArray");

    if(checkDoublePointer((const void**)jsonNode) == false || arrayValue == NULL)
        return *(bool*) handleError("jsonNode or arrayValue is NULL.", true, NULL, 1, NULL, 0, NULL);

    freeNodePointers(jsonNode);

    (*jsonNode)->type = array;
    (*jsonNode)->current = arrayValue;
    changeFieldName(jsonNode, fieldName);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Создание массива логических переменных.
JSON_NODE* createBoolArray(const bool* boolArray, int size) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createBoolArray") : sPush(&checkStack2, "createBoolArray");

    if(boolArray == NULL || checkNumber(size, 1, -1) == false)
        return (JSON_NODE*)handleError("boolArray is NULL or size has wrong value.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* first = NULL;                                          // Пустой объект массива.
    for(int i = 0; i<size; i++)                                       // Добавление в конец массива значений.
        addToJsonToNPos(&first, createBoolJsonNode(boolArray[i], "\0"), -1);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return first;
}

// Создание массива чисел.
JSON_NODE* createNumberArray(const double* numberArray, int size) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createNumberArray") : sPush(&checkStack2, "createNumberArray");

    if(numberArray == NULL || checkNumber(size, 1, -1) == false)
        return (JSON_NODE*)handleError("numberArray is NULL or size has wrong value.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createNumberJsonNode(numberArray[i], "\0"), -1);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return first;
}

// Создание массива строк.
JSON_NODE* createStringArray(const char** stringArray, int size) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createStringArray") : sPush(&checkStack2, "createStringArray");

    if(checkNumber(size, 1, -1) == false || checkArray((const void**)stringArray, size) == false)
        return (JSON_NODE*)handleError("stringArray is NULL or size has wrong value.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createStringJsonNode(stringArray[i], "\0"), -1);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return first;
}

// Создание массива объектов.
JSON_NODE* createObjectArray(JSON_NODE* objectArray, int size) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createObjectArray") : sPush(&checkStack2, "createObjectArray");

    if(objectArray == NULL || checkNumber(size, 1, -1) == false)
        return (JSON_NODE*)handleError("objectArray is NULL or size has wrong value.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createObjectJsonNode(&objectArray[i], "\0"), -1);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return first;
}

// Очистка массива.
bool deleteArray(JSON_NODE** array) {

    return removeJson(array);
}

// Добавление в массив на указанную позицию.
bool addToArrayToNPos(JSON_NODE** array, JSON_NODE* value, int index) {

    return addToJsonToNPos(array, value, index);
}

// Переход к элементу массива по указанному индексу.
JSON_NODE* findInArrayByIndex(JSON_NODE* array, int index) {

    return goToNJsonNode(array, index);
}

// Удаление из массива по указанному индексу.
bool removeFromArray(JSON_NODE** array, int index) {

    return removeNJsonNode(array, index);
}

// Создание узла объекта JSON по полю.
static JSON_NODE* createJsonNodeFromField(OBJECT_FIELD field) {

    switch(field.type) {                                       // Создание нужного узла в зависимости от указанного поля.

        case boolean:
            return createBoolJsonNode(field.value.boolVal, field.fieldName);
        case number:
            return createNumberJsonNode(field.value.numberVal, field.fieldName);
        case string:
            return createStringJsonNode(field.value.stringVal, field.fieldName);
        case array:
        case object:
            return createObject(field.size, field.value.current);
    }
}

// Создание объекта.
JSON_NODE* createObject(int countFields, OBJECT_FIELD* fields) {

    pthread_self() == thread1 ? sPush(&checkStack1, "createObject") : sPush(&checkStack2, "createObject");

    if(fields == NULL)
        return (JSON_NODE*)handleError("fields is NULL.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* first = NULL;                                    // Пустой объект.
    for(int i = 0; i<countFields; i++)                          // Добавление полей в объект.
        addToJsonToNPos(&first, createJsonNodeFromField(fields[i]), -1);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return first;
}

// Очистка объекта.
bool deleteObject(JSON_NODE** object) {

    return removeJson(object);
}

// Проверка на наличие в объекте поля.
bool hasObjectField(JSON_NODE* object, const char* fieldName) {

    return hasJsonField(object, fieldName);
}

// Поиск в объекте поля.
JSON_NODE* findByFieldName(JSON_NODE* object, const char* fieldName) {

    return goToJsonNodeByField(object, fieldName);
}

// Добавление в объект на заданную позицию.
bool addToObjectToNPos(JSON_NODE** objectNode, JSON_NODE* value, int position) {

    return addToJsonToNPos(objectNode, value, position);
}

// Удаление из объекта по полю.
bool removeFromObject(JSON_NODE** objectNode, const char* fieldName) {

    return removeJsonNodeByField(objectNode, fieldName);
}

// Вывод объекта JSON структурировано.
static void printJsonStructuredTabs(JSON_NODE* json, int tabulation) {

    JSON_NODE* temp = json;
    printf("{\n");

    while(temp != NULL) {                                                 // Обход всех элементов объекта JSON.

        TABULATION(tabulation);                                           // Табуляция, соответствующая уровню.
        printField(temp, tabulation, false);              // Вывод поля.

        temp = temp->next;
        if(temp != NULL)
            printf(",\n");
        else
            printf("\n");
    }

    if(tabulation != 1)
        TABULATION(tabulation - 1);

    printf("}\n");
}

// Вывод поля объекта JSON.
static void printField(JSON_NODE* jsonNode, int tabulation, bool isArray) {

    if(isArray == false)
        printf("\"%s\":", jsonNode->objectFieldName);              // Вывод названия поля, если это не объект массива.

    switch(jsonNode->type) {                                              // Вывод поля в виде, соответствующем типу значения.

        case boolean:
            if(jsonNode->boolVal == true)
                printf("\"true\"");
            else
                printf("\"false\"");
            break;
        case number:
            if(jsonNode->numberVal == (int)jsonNode->numberVal)
                printf("%.0f", jsonNode->numberVal);
            else
                printf("%g", jsonNode->numberVal);
            break;
        case string:
            printf("\"%s\"", jsonNode->stringVal);
            break;
        case object:
            printJsonStructuredTabs(jsonNode->current, tabulation + 1);
            break;
        case array:
            printf("\"%s\": [\n", jsonNode->objectFieldName);
            for(JSON_NODE* temp = jsonNode->current; temp != NULL; temp = temp->next) {
                
                TABULATION(tabulation + 1);
                printField(temp, tabulation + 1, true);

                if(temp->next != NULL)
                    printf(",\n");
                else
                    printf("\n");
            }

            TABULATION(tabulation);
            printf("]");
    }
}

// Вывод объекта JSON структурировано.
void printJsonStructured(JSON_NODE* json) {

    pthread_self() == thread1 ? sPush(&checkStack1, "printJsonStructured") : sPush(&checkStack2, "printJsonStructured");

    if(json != NULL)
        printJsonStructuredTabs(json, 1);
    else
        handleError("json is NULL.", false, NULL, 0, NULL, 0, NULL);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
}

// Вывод объекта JSON в виде строки.
void printJsonUnstructured(JSON_NODE* json) {

    pthread_self() == thread1 ? sPush(&checkStack1, "printJsonUnstructured") : sPush(&checkStack2, "printJsonUnstructured");

    if(json != NULL)
        printf("%s\n", parseJsonToString(json));
    else
        handleError("json is NULL.", false, NULL, 0, NULL, 0, NULL);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
}

// Добавление поля к строке с объектом JSON.
static void addField(JSON_NODE* jsonNode, char** result, bool isArray) {

    char* temp = (char*)malloc(80);
    switch(jsonNode->type) {                                                                 // Форматирование строки в нужном виде в соответствии с типом поля и добавление к результату.

        case boolean:
            if(jsonNode->boolVal == true) {
                if (isArray == true)
                    sprintf(temp, "\"true\"");
                else
                    sprintf(temp, "\"%s\":\"true\"", jsonNode->objectFieldName);
            }
            else {
                if(isArray == true)
                    sprintf(temp, "\"false\"");
                else
                    sprintf(temp, "\"%s\":\"false\"", jsonNode->objectFieldName);
            }
            addString(result, temp);
            break;
        case number:
            if(jsonNode->numberVal == (int)jsonNode->numberVal) {
                if(isArray == true)
                    sprintf(temp, "%.0f", jsonNode->numberVal);
                else
                    sprintf(temp, "\"%s\":%.0f", jsonNode->objectFieldName, jsonNode->numberVal);
            }
            else {
                if(isArray == true)
                    sprintf(temp, "%g", jsonNode->numberVal);
                else
                    sprintf(temp, "\"%s\":%g", jsonNode->objectFieldName, jsonNode->numberVal);
            }
            addString(result, temp);
            break;
        case string:
            if(isArray == true)
                sprintf(temp, "\"%s\"", jsonNode->stringVal);
            else
                sprintf(temp, "\"%s\":\"%s\"", jsonNode->objectFieldName, jsonNode->stringVal);
            addString(result, temp);
            break;
        case object:
            sprintf(temp, "\"%s\":", jsonNode->objectFieldName);
            addString(result, temp);
            addString(result, parseJsonToString(jsonNode->current));
            break;
        case array:
            sprintf(temp, "\"%s\":[", jsonNode->objectFieldName);
            addString(result, temp);
            for(JSON_NODE* tempNode = jsonNode->current; tempNode != NULL; tempNode = tempNode->next) {

                addField(tempNode, result, true);
                if(tempNode->next != NULL)
                    addString(result, ",");
            }
            addString(result, "]");
    }
    free(temp);
}

// Сериализация объекта JSON в строку.
char* parseJsonToString(JSON_NODE* json) {

    pthread_self() == thread1 ? sPush(&checkStack1, "parseJsonToString") : sPush(&checkStack2, "parseJsonToString");

    if(json == NULL)
        return handleError("json is NULL.", true, NULL, 0, NULL, 0, NULL);

    JSON_NODE* temp = json;                                 // Временный указатель для обхода полей.
    char* result = (char*)malloc(1);                   // Строка с результатом.
    result[0] = '\0';
    addString(&result, "{");

    while(temp != NULL) {

        addField(temp, &result, false);     // Добавление каждого поля.
        temp = temp->next;

        if(temp != NULL)
            addString(&result, ",");
    }
    addString(&result, "}");

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Чтение названия поля.
static char* parsePart(const char* str, int* i) {

    (*i)++;
    int j = *i;
    while(j < stringLength(str) && str[j] != '\"')      // Поиск конца строки.
        j++;

    char* result = (char*)malloc(j - *i);

    int a;
    for(a = 0; *i<j; a++, (*i)++)                             // Копирование результата.
        result[a] = str[*i];
    result[a] = '\0';

    return result;
}

// Проверка, что переменная типа логической переменной.
static bool isBool(const char* str, int i) {

    if(i + 4 > stringLength(str)) {

        if (str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e')
            return true;
        if (str[i] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' && str[i + 3] == 's' && str[i + 4] == 'e')
            return true;
    }
    return false;
}

// Проверка, что переменная типа числа.
static bool isNumber(char symbol) {

    if(symbol == '-' || (symbol >= '0' && symbol <= '9'))
        return true;
    return false;
}

// Проверка, что переменная типа строки.
static bool isString(char symbol) {

    return symbol == '\"';
}

// Проверка, что переменная типа массива.
static bool isArray(char symbol) {

    return symbol == '[';
}

// Чтение переменной типа объекта.
static void parseBool(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    (*i)++;
    if(str[*i] == 't' && str[*i+1] == 'r' && str[*i+2] == 'u' && str[*i+3] == 'e') {
        addToJsonToNPos(result, createBoolJsonNode(true, fieldName), -1);
        *i += 4;
    }
    else {
        addToJsonToNPos(result, createBoolJsonNode(false, fieldName), -1);
        *i += 5;
    }
}

// Чтение переменной типа строки.
static void parseString(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    char* temp = parsePart(str, i);                                                                              // Чтение значения.
    addToJsonToNPos(result, createStringJsonNode(temp, fieldName), -1);              // Добавление в объект.
    free(temp);
}

// Чтение переменной типа числа.
static void parseNumber(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    int j = *i;
    for(; isNumber(str[j]) || str[j] == '.'; j++);

    char* temp = copyStringFromIToSymbol(str, i, str[j]);                                               // Чтение значения.
    double number = stringToDouble(temp);                                                                    // Конвертирование в число.
    free(temp);

    addToJsonToNPos(result, createNumberJsonNode(number, fieldName), -1);          // Добавление в объект.
}

// Чтение переменной типа массива.
static void parseArray(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    JSON_NODE* array = NULL;

    (*i)++;
    if(isBool(str, *i + 1) == true) {                                                                          // Чтение массива соответствующего типа.
        while(*i < stringLength(str)) {

            parseBool(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;
            
            *i += 2;
        }
    }
    else if(isString(str[*i]) == true) {
        while(*i < stringLength(str)) {

            parseString(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    else if(isNumber(str[*i]) == true) {
        while(*i < stringLength(str)) {

            parseNumber(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    else if(isArray(str[*i]) == true) {
        while(*i < stringLength(str)) {

            parseArray(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    else {
        while(*i < stringLength(str)) {

            parseObject(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    (*i)++;

    addToJsonToNPos(result, createArrayJsonNode(array, fieldName), -1);               // Добавление в объект.
}

// Чтение переменной типа объект.
static void parseObject(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    JSON_NODE* object = NULL;

    (*i)++;
    while(*i < stringLength(str)) {

        char* tempFieldName = parsePart(str, i);
        (*i) += 2;

        if(isBool(str, *i + 1) == true)                                                                        // Чтение переменной в соответствии с типом.
            parseBool(&object, tempFieldName, str, i);
        else if(isString(str[*i]) == true)
            parseString(&object, tempFieldName, str, i);
        else if(isNumber(str[*i]) == true)
            parseNumber(&object, tempFieldName, str, i);
        else if(isArray(str[*i]) == true)
            parseArray(&object, tempFieldName, str, i);
        else 
            parseObject(&object, tempFieldName, str, i);

        free(tempFieldName);

        if(str[*i + 1] == '}')
            break;
        *i += 2;        
    }
    (*i)++;   

    addToJsonToNPos(result, createObjectJsonNode(object, fieldName), -1);            // Добавление в объект.
}

// Десериализация строки в объект JSON.
JSON_NODE* parseStringToJson(const char* str) {

    pthread_self() == thread1 ? sPush(&checkStack1, "parseStringToJson") : sPush(&checkStack2, "parseStringToJson");

    if(checkJsonString(str) != true)
        return (JSON_NODE*) handleError("str is NULL or has wrong format.", true, NULL, 2, NULL, 0, NULL);

    JSON_NODE* result = NULL;
    int strLen = stringLength(str), i = 1;
    
    while(i < strLen) {

        char* tempFieldName = parsePart(str, &i);                                                                 // Чтение названия поля.

        i+=2;                                                                                                     // Пропуск символа ":".
        if(isBool(str, i+1) == true)                                                                           // Чтение данных в соответствии с их типом и добавление в объект.
            parseBool(&result, tempFieldName, str, &i);
        else if(isString(str[i]) == true)
            parseString(&result, tempFieldName, str, &i);
        else if(isNumber(str[i]) == true)
            parseNumber(&result, tempFieldName, str, &i);
        else if(isArray(str[i]) == true)
            parseArray(&result, tempFieldName, str, &i);
        else
            parseObject(&result, tempFieldName, str, &i);

        free(tempFieldName);

        if(str[i+1] == '}')
            break;

        i+=2;                                                                                                     // Пропуск "," между объектами.
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}