#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>
#include "checkingFunctions.h"
#include "stringFunctions.h"

struct object_fields;

typedef union values {

    bool boolVal;
    double numberVal;
    char* stringVal;
    struct object_field* current;

} VALUES;

typedef struct object_field {

    char* fieldName;
    int type;
    VALUES value;
    int size;

} OBJECT_FIELD;

typedef enum json_types {

    null = 0,
    boolean,
    number,
    string,
    object,
    array

} JSON_TYPES;

typedef struct json_node {

    struct json_node* prev;
    struct json_node* next;
    
    int type;

    bool boolVal;
    double numberVal;
    char* stringVal;
    struct json_node* current; // object or array

    char* objectFieldName; 

} JSON_NODE;

#define JSON_MALLOC(n) ((JSON_NODE*)malloc((n)*sizeof(JSON_NODE)))
#define TABULATION(n) for (int i = 0; i<n; i++) printf("\t")

JSON_NODE* createJsonNode();
JSON_NODE* createBoolJsonNode(bool boolValue, const char* fieldName);
JSON_NODE* createNumberJsonNode(double numberValue, const char* fieldName);
JSON_NODE* createStringJsonNode(const char* stringValue, const char* fieldName);
JSON_NODE* createObjectJsonNode(JSON_NODE* objectValue, const char* fieldName);
JSON_NODE* createArrayJsonNode(JSON_NODE* arrayValue, const char* fieldName);

void removeJson(JSON_NODE** json);
void removeJsonNode(JSON_NODE** jsonNode);  // remove from list

JSON_NODE* goToNJsonNode(JSON_NODE* json, int n);
void addToJsonToNPos(JSON_NODE** json, JSON_NODE* node, int position);

void updateJsonNodeToBool(JSON_NODE** jsonNode, bool boolValue);
void updateJsonNodeToNumber(JSON_NODE** jsonNode, double numberValue);
void updateJsonNodeToString(JSON_NODE** jsonNode, const char* stringValue);
void updateJsonNodeToObject(JSON_NODE** jsonNode, JSON_NODE* objectValue);
void updateJsonNodeToArray(JSON_NODE** jsonNode, JSON_NODE* arrayValue);

JSON_NODE* createBoolArray(const bool* boolArray, int size);
JSON_NODE* createNumberArray(const double* numberArray, int size);
JSON_NODE* createStringArray(const char** stringArray, int size);
JSON_NODE* createObjectArray(JSON_NODE* objectArray, int size);

void deleteArray(JSON_NODE** array);
void addToArrayToNPos(JSON_NODE** array, JSON_NODE* value, int index);
void removeFromArray(JSON_NODE** arrayNode, int index);
JSON_NODE* findByIndex(JSON_NODE* array, int index);

JSON_NODE* createObject(int countFields, OBJECT_FIELD* fields);
void deleteObject(JSON_NODE** object);
bool hasObjectField(JSON_NODE* object, const char* fieldName);

void addToObjectToNPos(JSON_NODE** objectNode, JSON_NODE* value, int position);
void removeFromObject(JSON_NODE** objectNode, const char* fieldName);
JSON_NODE* findByFieldName(JSON_NODE* object, const char* fieldName);

void printJsonStructured(JSON_NODE* json);
void printJsonUnstructured(JSON_NODE* json);

char* parseJsonToString(JSON_NODE* json);
JSON_NODE* parseStringToJson(const char* string);