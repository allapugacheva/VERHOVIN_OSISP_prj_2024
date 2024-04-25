#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "checkingFunctions.h"

// Библиотека для работы с JSON.

struct object_fields;                                                                                  // Объявление структуры полей объекта.

typedef union values {                                                                                 // Значение поля объекта.

    bool boolVal;                                                                                      // Логическая переменная.
    double numberVal;                                                                                  // Число.
    char* stringVal;                                                                                   // Строка.
    struct object_field* current;                                                                      // Массив/объект.

} VALUES;

typedef struct object_field {

    char* fieldName;                                                                                   // Название поля.
    int type;                                                                                          // Тип данных поля.
    VALUES value;                                                                                      // Значение поля.
    int size;                                                                                          // Размер для массива/объекта.

} OBJECT_FIELD;

typedef enum json_types {                                                                              // Типы данных в JSON.

    null = 0,
    boolean,
    number,
    string,
    object,
    array

} JSON_TYPES;

typedef struct json_node {                                                                            // Узел JSON объекта.

    struct json_node* prev;
    struct json_node* next;
    
    int type;

    bool boolVal;
    double numberVal;
    char* stringVal;
    struct json_node* current;                                                                       // Массив/объект.

    char* objectFieldName; 

} JSON_NODE;

#define JSON_MALLOC(n) ((JSON_NODE*)malloc((n)*sizeof(JSON_NODE)))                                   // Макрос для создания объекта JSON.
#define TABULATION(n) for (int i = 0; i<(n); i++) printf("\t")                                       // Макрос для вывода табуляции при выводе объекта на экран.
#define IS_JSON_ERROR(node) (node).boolVal == error
#define JSON_ERROR &(JSON_NODE){.boolVal = error}

JSON_NODE* createJsonNode();                                                                         // Инициализация пустого узла.
JSON_NODE* createBoolJsonNode(bool boolValue, const char* fieldName);                                // Инициализация узла с логической переменной.
JSON_NODE* createNumberJsonNode(double numberValue, const char* fieldName);                          // Инициализация узла с числом.
JSON_NODE* createStringJsonNode(const char* stringValue, const char* fieldName);                     // Инициализация узла со строкой.
JSON_NODE* createObjectJsonNode(JSON_NODE* objectValue, const char* fieldName);                      // Инициализация узла с объектом.
JSON_NODE* createArrayJsonNode(JSON_NODE* arrayValue, const char* fieldName);                        // Инициализация узла с массивом.

bool removeJson(JSON_NODE** json);                                                                   // Очистить весь объект JSON.
bool removeJsonNode(JSON_NODE** jsonNode);                                                           // Удалить узел объекта JSON.
bool removeNJsonNode(JSON_NODE** json, int n);                                                       //test
bool removeJsonNodeByField(JSON_NODE** json, const char* fieldName);                                 //test

bool hasJsonField(JSON_NODE* json, const char* fieldName);                                           //test

JSON_NODE* goToJsonNodeByField(JSON_NODE* json, const char* fieldName);                              //test
JSON_NODE* goToNJsonNode(JSON_NODE* json, int n);                                                    // Переход к узлу с указанным номером.
bool addToJsonToNPos(JSON_NODE** json, JSON_NODE* node, int position);                               // Добавление узла на указанную позицию (-1 - в конец).

bool updateJsonNodeFieldName(JSON_NODE** jsonNode, const char* fieldName);
bool updateJsonNodeToBool(JSON_NODE** jsonNode, bool boolValue, const char* fieldName);              // Обновление типа узла на логическую переменную.
bool updateJsonNodeToNumber(JSON_NODE** jsonNode, double numberValue, const char* fieldName);        // Обновление типа узла на число.
bool updateJsonNodeToString(JSON_NODE** jsonNode, const char* stringValue, const char* fieldName);   // Обновление типа узла на строку.
bool updateJsonNodeToObject(JSON_NODE** jsonNode, JSON_NODE* objectValue, const char* fieldName);    // Обновление типа узла на объект.
bool updateJsonNodeToArray(JSON_NODE** jsonNode, JSON_NODE* arrayValue, const char* fieldName);      // Обновление типа узла на массив.

JSON_NODE* createBoolArray(const bool* boolArray, int size);                                         // Создание массива логических переменных.
JSON_NODE* createNumberArray(const double* numberArray, int size);                                   // Создание массива чисел.
JSON_NODE* createStringArray(const char** stringArray, int size);                                    // Создание массива строк.
JSON_NODE* createObjectArray(JSON_NODE* objectArray, int size);                                      // Создание массива объектов.

bool deleteArray(JSON_NODE** array);                                                                 // Очистить массив.
bool addToArrayToNPos(JSON_NODE** array, JSON_NODE* value, int index);                               // Добавить в массив на указанную позицию (-1 - в конец).
bool removeFromArray(JSON_NODE** arrayNode, int index);                                              // Удалить из массива с указанной позиции.
JSON_NODE* findInArrayByIndex(JSON_NODE* array, int index);                                          // Поиск по индексу элемента массива.

JSON_NODE* createObject(int countFields, OBJECT_FIELD* fields);                                      // Создание объекта.
bool deleteObject(JSON_NODE** object);                                                               // Удаление объекта.
bool hasObjectField(JSON_NODE* object, const char* fieldName);                                       // Проверка на наличие поля в объекте.

bool addToObjectToNPos(JSON_NODE** objectNode, JSON_NODE* value, int position);                      // Добавление к объекту на указанную позицию.
bool removeFromObject(JSON_NODE** objectNode, const char* fieldName);                                // Удаление из объекта по полю.
JSON_NODE* findInObjectByFieldName(JSON_NODE* object, const char* fieldName);                        // Поиск по полю объекта.

void printJsonStructured(JSON_NODE* json);                                                           // Вывод объекта JSON структурированного.
void printJsonUnstructured(JSON_NODE* json);                                                         // Вывод объекта JSON в виде строки.

char* parseJsonToString(JSON_NODE* json);                                                            // Сериализация объекта JSON в строку.
JSON_NODE* parseStringToJson(const char* string);                                                    // Десериализация строки в объект JSON.

//----------------------------------------------------------------------------------------------------------------------

static void makeNullFields(JSON_NODE** node);                                                        // Инициализация указателей объекта JSON значениями NULL.
static void freeNodePointers(JSON_NODE** jsonNode);                                                  // Очистка указателей объекта JSON.
static void changeFieldName(JSON_NODE** jsonNode, const char* fieldName);                            // Изменить имя поля объекта JSON.

static void connectNode(JSON_NODE** jsonNode, JSON_NODE** prevNode, JSON_NODE** nextNode);           // Связать 2 узла объекта JSON.

static int jsonLength(JSON_NODE* json);                                                              // Длина объекта JSON.

static JSON_NODE* createJsonNodeFromField(OBJECT_FIELD field);                                       // Создание узла объекта JSON из структуры OBJECT_FIELD.

static void printJsonStructuredTabs(JSON_NODE* json, int tabulation);                                // Вывод объекта JSON структурировано.
static void printField(JSON_NODE* jsonNode, int tabulation, bool isArray);                           // Вывод одного поля.

static void addField(JSON_NODE* jsonNode, char** result, bool isArray);                              // Добавление одного поля в строку с объектом JSON.

static char* parsePart(const char* str, int* i);                                                     // Копирование части строки до первого вхождения символа """.

static bool isBool(const char* str, int i);                                                          // Проверка, что в строке записан тип логической переменной.
static bool isNumber(char symbol);                                                                   // Проверка, что в строке записан тип числа.
static bool isString(char symbol);                                                                   // Проверка, что в строке записан тип строки.
static bool isArray(char symbol);                                                                    // Проверка, что в строке записан тип массив.

static void parseBool(JSON_NODE** result, const char* fieldName, const char* str, int* i);           // Чтение логической переменной из строки.
static void parseString(JSON_NODE** result, const char* fieldName, const char* str, int* i);         // Чтение строки из строки.
static void parseNumber(JSON_NODE** result, const char* fieldName, const char* str, int* i);         // Чтение номера из строки.
static void parseArray(JSON_NODE** result, const char* fieldName, const char* str, int* i);          // Чтение массива из строки.
static void parseObject(JSON_NODE** result, const char* fieldName, const char* str, int* i);         // Чтение объекта из строки.