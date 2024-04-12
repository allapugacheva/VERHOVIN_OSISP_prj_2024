#include "json.h"

static void makeNullFields(JSON_NODE** node) {

    (*node)->current = NULL;
    (*node)->next = NULL;
    (*node)->objectFieldName = NULL;
    (*node)->prev = NULL;
    (*node)->stringVal = NULL;
}

static void freeNodePointers(JSON_NODE** jsonNode) {

    if((*jsonNode)->type == string)
        free((*jsonNode)->stringVal);

    if((*jsonNode)->type == array || (*jsonNode)->type == object)
        removeJson(&(*jsonNode)->current);
}

JSON_NODE* createJsonNode() {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->type = null;

    return newNode;
}
JSON_NODE* createBoolJsonNode(bool boolValue, const char* fieldName) {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->boolVal = boolValue;
    newNode->type = boolean;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createNumberJsonNode(double numberValue, const char* fieldName) {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->numberVal = numberValue;
    newNode->type = number;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createStringJsonNode(const char* stringValue, const char* fieldName) {

    checkPointer(stringValue, "createStringJsonNode", "stringValue");

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    copyString(&(newNode->stringVal), stringValue);
    newNode->type = string;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createObjectJsonNode(JSON_NODE* objectValue, const char* fieldName) {

    checkPointer(objectValue, "createObjectJsonNode", "objectValue");

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = objectValue;
    newNode->type = object;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createArrayJsonNode(JSON_NODE* arrayValue, const char* fieldName) {

    checkPointer(arrayValue, "createArrayJsonNode", "arrayValue");

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = arrayValue;
    newNode->type = array;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}

void removeJsonNode(JSON_NODE** jsonNode) {  // free

    checkDoublePointer((void**)jsonNode, "removeJsonNode", "jsonNode");

    bool moveHead = false;

    if((*jsonNode)->next != NULL)
        (*jsonNode)->next->prev = (*jsonNode)->prev;
    if((*jsonNode)->prev != NULL)
        (*jsonNode)->prev->next = (*jsonNode)->next;
    else 
        moveHead = true;

    freeNodePointers(jsonNode);

    if(moveHead) {

        JSON_NODE* temp = *jsonNode;
        *jsonNode = (*jsonNode)->next;

        free(temp);
    } else {

        free(*jsonNode);
        *jsonNode = NULL;
    }
}
void removeJson(JSON_NODE** json) {

    checkDoublePointer((void**)json, "removeJson", "json");

    while (*json != NULL)
        removeJsonNode(json);

}

static void connectNode(JSON_NODE** jsonNode, JSON_NODE** prevNode, JSON_NODE** nextNode) {

    if(nextNode != NULL)
        (*jsonNode)->next = *nextNode;
    else
        (*jsonNode)->next = NULL;
    if(nextNode != NULL && *nextNode != NULL)
        (*nextNode)->prev = *jsonNode;

    if(prevNode != NULL)
        (*jsonNode)->prev = *prevNode;
    else
        (*jsonNode)->prev = NULL;
    if(prevNode != NULL && *prevNode != NULL)
        (*prevNode)->next = *jsonNode;
}

static int jsonLength(JSON_NODE* json) {

    int length = 0;
    for(JSON_NODE* temp = json; temp != NULL; temp = temp->next, length++);

    return length;
}

JSON_NODE* goToNJsonNode(JSON_NODE* json, int n) {

    checkPointer(json, "goToNJsonNode", "json");
    checkNumber(n, -1, jsonLength(json), "goToNJsonNode");

    JSON_NODE* temp = json;

    int count = 1;
    while(temp->next != NULL && count++ != n)
        temp = temp->next;

    return temp;
}

// to array, object also
void addToJsonToNPos(JSON_NODE** json, JSON_NODE* node, int position) {

    if (json != NULL) {

        checkPointer(node, "addToJsonToNPos", "node");

        if (position < -1 || (*json != NULL && position > jsonLength(*json)) || (*json == NULL && position > 1)) {
            printf("Error in 'addToJsonToNPos' - wrong position\n");
            exit(0);
        }

        if (*json == NULL)
            *json = node;
        else {
            JSON_NODE *nNode = goToNJsonNode(*json, position);

            if (position == -1)
                connectNode(&node, &nNode, NULL);
            else
                connectNode(&node, &nNode->prev, &nNode);

            if (position == 1)
                *json = (*json)->prev;
        }
    } else {
        printf("Error in 'addToJsonToNPos' - NULL json\n");
        exit(0);
    }
}

void updateJsonNodeToBool(JSON_NODE** jsonNode, bool boolValue) {

    checkDoublePointer((void**)jsonNode, "updateJsonNodeToBool", "jsonNode");
    freeNodePointers(jsonNode);

    (*jsonNode)->type = boolean;
    (*jsonNode)->boolVal = boolValue;
}
void updateJsonNodeToNumber(JSON_NODE** jsonNode, double numberValue) {

    checkDoublePointer((void**)jsonNode, "updateJsonNodeToNumber", "jsonNode");
    freeNodePointers(jsonNode);

    (*jsonNode)->type = number;
    (*jsonNode)->numberVal = numberValue;
    
}
void updateJsonNodeToString(JSON_NODE** jsonNode, const char* stringValue) {

    checkDoublePointer((void**)jsonNode, "updateJsonNodeToString", "jsonNode");
    freeNodePointers(jsonNode);

    (*jsonNode)->type = string;
    copyString(&(*jsonNode)->stringVal, stringValue);
}
void updateJsonNodeToObject(JSON_NODE** jsonNode, JSON_NODE* objectValue) {

    checkDoublePointer((void**)jsonNode, "updateJsonNodeToObject", "jsonNode");
    checkPointer(objectValue, "updateJsonNodeToObject", "objectValue");
    freeNodePointers(jsonNode);

    (*jsonNode)->type = object;
    (*jsonNode)->current = objectValue;
}
void updateJsonNodeToArray(JSON_NODE** jsonNode, JSON_NODE* arrayValue) {

    checkDoublePointer((void**)jsonNode, "updateJsonNodeToArray", "jsonNode");
    checkPointer(arrayValue, "updateJsonNodeToArray", "arrayValue");
    freeNodePointers(jsonNode);

    (*jsonNode)->type = array;
    (*jsonNode)->current = arrayValue;
}

JSON_NODE* createBoolArray(const bool* boolArray, int size) {

    checkPointer(boolArray, "createBoolArray", "boolArray");
    checkNumber(size, 1, -1, "createBoolArray");

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createBoolJsonNode(boolArray[i], NULL), -1);

    return first;
}
JSON_NODE* createNumberArray(const double* numberArray, int size) {

    checkPointer(numberArray, "createNumberArray", "numberArray");
    checkNumber(size, 1, -1, "createNumberArray");

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createNumberJsonNode(numberArray[i], NULL), -1);

    return first;
}
JSON_NODE* createStringArray(const char** stringArray, int size) {

    checkPointer(stringArray, "createStringArray", "stringArray");
    checkNumber(size, 1, -1, "createStringArray");

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createStringJsonNode(stringArray[i], NULL), -1);

    return first;
}
JSON_NODE* createObjectArray(JSON_NODE* objectArray, int size) {

    checkPointer(objectArray, "createObjectArray", "objectArray");
    checkNumber(size, 1, -1, "createObjectArray");

    JSON_NODE* first = NULL;
    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createObjectJsonNode(&objectArray[i], NULL), -1);

    return first;
}
void deleteArray(JSON_NODE** array) {

    removeJson(array);
}

static int arraySize(JSON_NODE* array) {

    int size = 0;
    for (JSON_NODE *temp = array; temp != NULL; temp = temp->next, size++);

    return size;
}

void addToArrayToNPos(JSON_NODE** array, JSON_NODE* value, int index) {

    addToJsonToNPos(array, value, index);
}

JSON_NODE* findByIndex(JSON_NODE* array, int index) {

    checkPointer(array, "findByIndex", "array");
    checkNumber(index, 0, arraySize(array) - 1, "findByIndex");

    JSON_NODE* temp = array;
    while(temp != NULL && index-- > 0)
        temp = temp->next;

    return temp;
}

void removeFromArray(JSON_NODE** array, int index) {

    checkDoublePointer((void**)array, "removeFromArray", "array");
    checkNumber(index, 0, arraySize(*array), "removeFromArray");

    JSON_NODE* temp = goToNJsonNode(*array, index);
    removeJsonNode(&temp);
}

static JSON_NODE* createJsonNodeFromField(OBJECT_FIELD field) {

    switch(field.type) {

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

    printf("Error in 'createJsonNOdeFromField' - no available type\n");
    exit(0);
}

JSON_NODE* createObject(int countFields, OBJECT_FIELD* fields) {

    checkPointer(fields, "createObject", "fields");

    JSON_NODE* first = createJsonNodeFromField(fields[0]);
    for(int i = 1; i<countFields; i++)
        addToJsonToNPos(&first, createJsonNodeFromField(fields[i]), -1);

    return first;
}
void deleteObject(JSON_NODE** object) {

    removeJson(object);
}

bool hasObjectField(JSON_NODE* object, const char* fieldName) {

    checkPointer(object, "hasObjectField", "object");
    checkPointer(fieldName, "hasObjectField", "fieldName");

    JSON_NODE* temp = object;
    while(temp != NULL) {
        if(compareString(temp->objectFieldName, fieldName) == 0)
            return true;
        temp = temp->next;
    }

    return false;
}
JSON_NODE* findByFieldName(JSON_NODE* object, const char* fieldName) {

    checkPointer(object, "findByFieldName", "object");
    checkPointer(fieldName, "findByFieldName", "fieldName");

    JSON_NODE* temp = object;
    while(temp != NULL) {
        if(compareString(temp->objectFieldName, fieldName) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

void addToObjectToNPos(JSON_NODE** objectNode, JSON_NODE* value, int position) {

    checkPointer(value, "addToObjectToNPos", "value");
    checkPointer(value->objectFieldName, "addToObjectToNPos", "value->objectFieldName");

    addToJsonToNPos(objectNode, value, position);
}
void removeFromObject(JSON_NODE** objectNode, const char* fieldName) {

    checkDoublePointer((void**)objectNode, "removeFromObject", "objectNode");
    checkPointer(fieldName, "removeFromObject", "fieldName");

    JSON_NODE* temp = findByFieldName(*objectNode, fieldName);
    removeJsonNode(&temp);
}

static void printField(JSON_NODE* jsonNode, int tabulation);

static void printJsonStructuredTabs(JSON_NODE* json, int tabulation) {

    JSON_NODE* temp = json;
    printf("{\n");

    while(temp != NULL) {

        TABULATION(tabulation);
        printField(temp, tabulation);

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

static void printArrayValue(JSON_NODE* jsonNode, int tabulation) {

    switch(jsonNode->type) {

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
            printJsonStructuredTabs(jsonNode->current, tabulation);
            break;
        case array:
            printf("\"%s\": [\n", jsonNode->objectFieldName);
            for(JSON_NODE* temp = jsonNode->current; temp != NULL; temp = temp->next) {
                
                TABULATION(tabulation);
                printArrayValue(jsonNode, tabulation + 1);

                if(temp->next != NULL)
                    printf(",\n");
                else
                    printf("\n");
            }

            TABULATION(tabulation);
            printf("]\n");
    }

}

static void printField(JSON_NODE* jsonNode, int tabulation) {

    switch(jsonNode->type) {

        case boolean:
            if(jsonNode->boolVal == true)
                printf("\"%s\":\"true\"", jsonNode->objectFieldName);
            else
                printf("\"%s\":\"false\"", jsonNode->objectFieldName);
            break;
        case number:
            if(jsonNode->numberVal == (int)jsonNode->numberVal)
                printf("\"%s\":%.0f", jsonNode->objectFieldName, jsonNode->numberVal);
            else
                printf("\"%s\":%g", jsonNode->objectFieldName, jsonNode->numberVal);
            break;
        case string:
            printf("\"%s\":\"%s\"", jsonNode->objectFieldName, jsonNode->stringVal);
            break;
        case object:
            printf("\"%s\":", jsonNode->objectFieldName);
            printJsonStructuredTabs(jsonNode->current, tabulation + 1);
            break;
        case array:
            printf("\"%s\": [\n", jsonNode->objectFieldName);
            for(JSON_NODE* temp = jsonNode->current; temp != NULL; temp = temp->next) {
                
                TABULATION(tabulation + 1);
                printArrayValue(temp, tabulation + 1);

                if(temp->next != NULL)
                    printf(",\n");
                else
                    printf("\n");
            }

            TABULATION(tabulation);
            printf("]");
    }
}

void printJsonStructured(JSON_NODE* json) {

    checkPointer(json, "printJsonStructured", "json");
    
    printJsonStructuredTabs(json, 1);
}
void printJsonUnstructured(JSON_NODE* json) {

    checkPointer(json, "printJsonUnstructured", "json");

    printf("%s\n", parseJsonToString(json));
}

static void addArrayValue(JSON_NODE* jsonNode, char** result) {

    char* temp = (char*)malloc(80);
    switch(jsonNode->type) {

        case boolean:
            if(jsonNode->boolVal == true)
                sprintf(temp, "\"true\"");
            else
                sprintf(temp, "\"false\"");
            addString(result, temp);
            break;
        case number:
            if(jsonNode->numberVal == (int)jsonNode->numberVal)
                sprintf(temp, "%.0f", jsonNode->numberVal);
            else
                sprintf(temp, "%g", jsonNode->numberVal);
            addString(result, temp);
            break;
        case string:
            sprintf(temp, "\"%s\"", jsonNode->stringVal);
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

                addArrayValue(tempNode, result);
                if(tempNode->next != NULL)
                    addString(result, ",");
            }
            addString(result, "]");
    }
    free(temp);
}

static void addField(JSON_NODE* jsonNode, char** result) {

    char* temp = (char*)malloc(80);
    switch(jsonNode->type) {

        case boolean:
            if(jsonNode->boolVal == true)
                sprintf(temp, "\"%s\":\"true\"", jsonNode->objectFieldName);
            else
                sprintf(temp, "\"%s\":\"false\"", jsonNode->objectFieldName);
            addString(result, temp);
            break;
        case number:
            if(jsonNode->numberVal == (int)jsonNode->numberVal)
                sprintf(temp, "\"%s\":%.0f", jsonNode->objectFieldName, jsonNode->numberVal);
            else
                sprintf(temp, "\"%s\":%g", jsonNode->objectFieldName, jsonNode->numberVal);
            addString(result, temp);
            break;
        case string:
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

                addArrayValue(tempNode, result);
                if(tempNode->next != NULL)
                    addString(result, ",");
            }
            addString(result, "]");
    }
    free(temp);
}

char* parseJsonToString(JSON_NODE* json) {

    checkPointer(json, "parseJsonToString", "json");

    JSON_NODE* temp = json;
    char* result = (char*)malloc(1);
    result[0] = '\0';
    addString(&result, "{");

    while(temp != NULL) {

        addField(temp, &result);
        temp = temp->next;

        if(temp != NULL)
            addString(&result, ",");
    }
    addString(&result, "}");

    return result;
}

static char* parsePart(const char* str, int* i) {

    (*i)++;
    int j = *i;
    while(j < stringLength(str) && str[j] != '\"')
        j++;

    if(j == stringLength(str)) {
        printf("Error in 'parsePart' - j out of borders\n");
        exit(0);
    }

    char* result = (char*)malloc(j - *i);

    int a;
    for(a = 0; *i<j; a++, (*i)++)
        result[a] = str[*i];
    result[a] = '\0';

    return result;
}
static bool isBool(const char* str, int i) {

    if(i + 4 > stringLength(str)) {

        if (str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e')
            return true;
        if (str[i] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' && str[i + 3] == 's' && str[i + 4] == 'e')
            return true;
    }
    return false;
}
static bool isNumber(const char symbol) {

    if(symbol == '-' || (symbol >= '0' && symbol <= '9'))
        return true;
    return false;
}
static bool isString(const char symbol) {

    return symbol == '\"';
}
static bool isArray(const char symbol) {

    return symbol == '[';
}
static bool isObject(const char symbol) {

    return symbol == '{';
}

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
static void parseString(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    addToJsonToNPos(result, createStringJsonNode(parsePart(str, i), fieldName), -1);
}
static void parseNumber(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    bool negative = false, integer = true;
    double number = 0, divisor = 1;
    while(isNumber(str[*i]) || str[*i] == '.') {

        if(str[*i] == '-') {
            negative = true;
            (*i)++;
            continue;
        }
        if(str[*i] == '.') {
            integer = false;
            (*i)++;
            continue;
        }
        number = number * 10 + str[*i] - '0';

        if(!integer)
            divisor *= 10;
        (*i)++;
    }

    if(!integer)
        number /= divisor;
    if(negative)
        number *= -1;
    (*i)--;

    addToJsonToNPos(result, createNumberJsonNode(number, fieldName), -1);
}

static void parseObject(JSON_NODE** result, const char* fieldName, const char* str, int* i);

static void parseArray(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    JSON_NODE* array = NULL;

    (*i)++;
    if(isBool(str, *i + 1)) {
        while(*i < stringLength(str)) {

            parseBool(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;
            
            *i += 2;
        }
    }
    else if(isString(str[*i])) {
        while(*i < stringLength(str)) {

            parseString(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    else if(isNumber(str[*i])) {
        while(*i < stringLength(str)) {

            parseNumber(&array, fieldName, str, i);
            if(str[*i + 1] == ']')
                break;

            *i += 2;
        }
    }
    else if(isArray(str[*i])) {
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

    addToJsonToNPos(result, createArrayJsonNode(array, fieldName), -1);
}
static void parseObject(JSON_NODE** result, const char* fieldName, const char* str, int* i) {

    JSON_NODE* object = NULL;

    (*i)++;
    while(*i < stringLength(str)) {

        char* tempFieldName = parsePart(str, i);
        (*i) += 2;

        if(isBool(str, *i + 1))
            parseBool(&object, tempFieldName, str, i);
        else if(isString(str[*i]))
            parseString(&object, tempFieldName, str, i);
        else if(isNumber(str[*i]))
            parseNumber(&object, tempFieldName, str, i);
        else if(isArray(str[*i])) 
            parseArray(&object, tempFieldName, str, i);
        else 
            parseObject(&object, tempFieldName, str, i);

        if(str[*i + 1] == '}')
            break;
        *i += 2;        
    }
    (*i)++;   

    addToJsonToNPos(result, createObjectJsonNode(object, fieldName), -1); 
}

JSON_NODE* parseStringToJson(const char* str) {

    checkPointer(str, "parseStringToJson", "str");

    JSON_NODE* result = NULL;
    int strLen = stringLength(str), i = 1;
    
    while(i < strLen) {

        char* tempFieldName = parsePart(str, &i);

        i+=2; // skip :
        if(isBool(str, i+1))
            parseBool(&result, tempFieldName, str, &i);
        else if(isString(str[i]))
            parseString(&result, tempFieldName, str, &i);
        else if(isNumber(str[i]))
            parseNumber(&result, tempFieldName, str, &i);
        else if(isArray(str[i]))
            parseArray(&result, tempFieldName, str, &i);
        else
            parseObject(&result, tempFieldName, str, &i);
        if(str[i+1] == '}')
            break;

        i+=2; // skip ,
    }
    return result;
}