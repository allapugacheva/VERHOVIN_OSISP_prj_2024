#include "json.h"

static void makeNullFields(JSON_NODE** node) {

    if(node != NULL && *node != NULL) {

        (*node)->current = NULL;
        (*node)->next = NULL;
        (*node)->objectFieldName = NULL;
        (*node)->prev = NULL;
        (*node)->stringVal = NULL;
    } else {
        printf("Error in 'makeNullFields' - NULL node\n");
        exit(0);
    }
}

static int stringLength(char* string) {

    if(string == NULL) {
        printf("Error in 'stringLenght' - NULL string\n");
        exit(0);
    }

    int lenght = 0;
    while(string[lenght]!='\0')
        lenght++;
    
    return lenght;
}

static void copyString(char** to, char* from) {

    if(to == NULL) {
        printf("Error in 'copyString' - NULL to\n");
        exit(0);
    }
    if(from == NULL) {
        printf("Error in 'copyString' - NULL from\n");
        exit(0);
    }

    *to = (char*)malloc(stringLength(from) + 1);

    for(int i = 0; i<=stringLength(from); i++)
        (*to)[i] = from[i];
}

static void freeNodePointers(JSON_NODE** jsonNode) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'freeNodePointers' - NULL jsonNode\n");
        exit(0);
    }

    if((*jsonNode)->type == string)
        free((*jsonNode)->stringVal);

    if((*jsonNode)->type == array)
        removeJson(&(*jsonNode)->current);

    if((*jsonNode)->type == object)
        removeJson(&(*jsonNode)->current);
}

static int compareString(char* str1, char* str2) {

    if(str1 == NULL) {
        printf("Error in 'compareString' - NULL str1\n");
        exit(0);
    }
    if(str2 == NULL) {
        printf("Error in 'compareString' - NULL str2\n");
        exit(0);
    }

    int len1 = stringLength(str1), len2 = stringLength(str2), minimal = len1 < len2 ? len1 : len2;

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

JSON_NODE* createJsonNode() {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->type = null;

    return newNode;
}
JSON_NODE* createBoolJsonNode(bool boolValue, char* fieldName) {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->boolVal = boolValue;
    newNode->type = boolean;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createNumberJsonNode(double numberValue, char* fieldName) {

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->numberVal = numberValue;
    newNode->type = number;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createStringJsonNode(char* stringValue, char* fieldName) {

    if(stringValue == NULL) {
        printf("Error in 'createStringJsonNode' - NULL stringValue\n");
        exit(0);
    }

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    copyString(&(newNode->stringVal), stringValue);
    newNode->type = string;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createObjectJsonNode(JSON_NODE* objectValue, char* fieldName) {

    if(objectValue == NULL) {
        printf("Error in 'createObjectJsonNode' - NULL objectValue\n");
        exit(0);
    }

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = objectValue;
    newNode->type = object;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}
JSON_NODE* createArrayJsonNode(JSON_NODE* arrayValue, char* fieldName) {

    if(arrayValue == NULL) {
        printf("Error in 'createArrayJsonNode' - NULL arrayValue\n");
        exit(0);
    }

    JSON_NODE* newNode = JSON_MALLOC(1);
    makeNullFields(&newNode);

    newNode->current = arrayValue;
    newNode->type = array;

    if(fieldName != NULL)
        copyString(&newNode->objectFieldName, fieldName);

    return newNode;
}

void removeJsonNode(JSON_NODE** jsonNode) {  // free

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'removeJsonNode' - NULL jsonNode\n");
        exit(0);
    }

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

    if(json != NULL && *json != NULL) {

        while(*json != NULL)
            removeJsonNode(json);

    } else {
        printf("Error in 'removeJson' - NULL json\n");
        exit(0);
    }
}

static void connectNode(JSON_NODE** jsonNode, JSON_NODE** prevNode, JSON_NODE** nextNode) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'connectNode' - NULL jsonNode\n");
        exit(0);
    }

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

    if(json == NULL) {
        printf("Error in 'jsonLength' - NULL json\n");
        exit(0);
    }

    int length = 0;
    for(JSON_NODE* temp = json; temp != NULL; temp = temp->next, length++);

    return length;
}

JSON_NODE* goToNJsonNode(JSON_NODE* json, int n) {

    if(json == NULL) {
        printf("Error in 'goToNJsonNode' - NULL json\n");
        exit(0);
    }
    if(n < -1 || n > jsonLength(json)) {
        printf("Error in 'goToNJsonNode' - wrong position\n");
        exit(0);
    }

    JSON_NODE* temp = json;

    int count = 1;
    while(temp->next != NULL && count++ != n)
        temp = temp->next;

    return temp;
}

// to array also
void addToJsonToNPos(JSON_NODE** json, JSON_NODE* node, int position) {

    if(node == NULL) {
        printf("Error in 'addToJsonToNPos' - NULL node\n");
        exit(0);
    }
    if(position < -1 || (*json != NULL && position > jsonLength(*json))) {
        printf("Error in 'addToJsonToNPos' - wrong position\n");
        exit(0);
    }

    if(json != NULL) {

        if(*json == NULL)
            *json = node;
        else {
            JSON_NODE* nNode = goToNJsonNode(*json, position);

            if(position == -1)
                connectNode(&node, &nNode, NULL);
            else
                connectNode(&node, &nNode->prev, &nNode);
        
            if(position == 1)
                *json = (*json)->prev;
        }
    } else {
        printf("Error in 'addToJsonToNPos' - NULL json\n");
        exit(0);
    }
} 

void updateJsonNodeToBool(JSON_NODE** jsonNode, bool boolValue) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'updateJsonNodeToBool' - NULL jsonNode\n");
        exit(0);
    }

    freeNodePointers(jsonNode);

    (*jsonNode)->type = boolean;
    (*jsonNode)->boolVal = boolValue;
}
void updateJsonNodeToNumber(JSON_NODE** jsonNode, double numberValue) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'updateJsonNodeToNumber' - NULL jsonNode\n");
        exit(0);
    }    

    freeNodePointers(jsonNode);

    (*jsonNode)->type = number;
    (*jsonNode)->numberVal = numberValue;
    
}
void updateJsonNodeToString(JSON_NODE** jsonNode, char* stringValue) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'updateJsonNodeToString' - NULL jsonNode\n");
        exit(0);
    }    

    freeNodePointers(jsonNode);

    (*jsonNode)->type = string;
    copyString(&(*jsonNode)->stringVal, stringValue);
}
void updateJsonNodeToObject(JSON_NODE** jsonNode, JSON_NODE* objectValue) {

    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'updateJsonNodeToObject' - NULL jsonNode\n");
        exit(0);
    }    
    if(objectValue == NULL) {
        printf("Error in 'updateJsonNodeToObject' - NULL objectValue\n");
        exit(0);
    }
    
    freeNodePointers(jsonNode);

    (*jsonNode)->type = object;
    (*jsonNode)->current = objectValue;
}
void updateJsonNodeToArray(JSON_NODE** jsonNode, JSON_NODE* arrayValue) {
    
    if(jsonNode == NULL || *jsonNode == NULL) {
        printf("Error in 'updateJsonNodeToArray' - NULL jsonNode\n");
        exit(0);
    }    
    if(arrayValue == NULL) {
        printf("Error in 'updateJsonNodeToArray' - NULL arrayValue\n");
        exit(0);
    }

    freeNodePointers(jsonNode);

    (*jsonNode)->type = array;
    (*jsonNode)->current = arrayValue;
}

JSON_NODE* createBoolArray(bool* boolArray, int size) {

    if(boolArray == NULL) {
        printf("Error in 'createBoolArray' - NULL boolArray\n");
        exit(0);
    }
    if(size <= 0) {
        printf("Error in 'createBoolArray' - wrong size\n");
        exit(0);
    }

    JSON_NODE* first = NULL;

    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createBoolJsonNode(boolArray[i], NULL), -1);

    return first;
}
JSON_NODE* createNumberArray(double* numberArray, int size) {

    if(numberArray == NULL) {
        printf("Error in 'createNumberArray' - NULL numberArray\n");
        exit(0);
    }
    if(size <= 0) {
        printf("Error in 'createNumberArray' - wrong size\n");
        exit(0);
    }

    JSON_NODE* first = NULL;

    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createNumberJsonNode(numberArray[i], NULL), -1);

    return first;
}
JSON_NODE* createStringArray(char** stringArray, int size) {

    if(stringArray == NULL) {
        printf("Error in 'createStringArray' - NULL stringArray\n");
        exit(0);
    }
    if(size <= 0) {
        printf("Error in 'createStringArray' - wrong size\n");
        exit(0);
    }    

    JSON_NODE* first = NULL;

    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createStringJsonNode(stringArray[i], NULL), -1);

    return first;
}
JSON_NODE* createObjectArray(JSON_NODE* objectArray, int size) {

    if(objectArray == NULL) {
        printf("Error in 'createObjectArray' - NULL objectArray\n");
        exit(0);
    }
    if(size <= 0) {
        printf("Error in 'createObjectArray' - wrong size\n");
        exit(0);
    }    

    JSON_NODE* first = NULL;

    for(int i = 0; i<size; i++)
        addToJsonToNPos(&first, createObjectJsonNode(&objectArray[i], NULL), -1);

    return first;
}
void deleteArray(JSON_NODE** array) {
    
    if(array == NULL || *array == NULL) {
        printf("Error in 'deleteArray' - NULL array\n");
        exit(0);
    }

    removeJson(array);
}

static int arraySize(JSON_NODE* array);

void addToArrayToNPos(JSON_NODE** array, JSON_NODE* value, int index) {

    if(array == NULL) {
        printf("Error in 'addToArrayToNPos' - NULL array\n");
        exit(0);
    }
    if(value == NULL) {
        printf("Error in 'addToArrayToNPos' - NULL value\n");
        exit(0);
    }
    if(index < -1 || index > arraySize(*array)) {
        printf("Error in 'addToArrayToNPos' - wrong index\n");
        exit(0);
    }

    addToJsonToNPos(array, value, index);
}

static int arraySize(JSON_NODE* array) {

    if(array != NULL && array->prev == NULL) {

        int size = 0;

        for(JSON_NODE* temp = array; temp != NULL; temp = temp->next, size++);

        return size;
    } else {
        printf("Error in 'arraySize' - NULL array\n");
        exit(0);
    }
}

JSON_NODE* findByIndex(JSON_NODE* array, int index) {

    if(array == NULL) {
        printf("Error in 'findByIndex' - NULL arrayNode\n");
        exit(0);
    }
    if(index < 0 || index >= arraySize(array)) {
        printf("Error in 'findByIndex' - wrong index\n");
        exit(0);
    }

    JSON_NODE* temp = array;

    while(temp != NULL && index-- > 0)
        temp = temp->next;

    return temp;
}

void removeFromArray(JSON_NODE** array, int index) {

    if(array == NULL || *array == NULL) {
        printf("Error in 'removeFromArray' - NULL arrayNode\n");
        exit(0);
    }
    if(index < 0 || index > arraySize(*array)) {
        printf("Error in 'removeFromArray' - wrong index\n");
        exit(0);
    }

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

    if(fields == NULL) {
        printf("Error in 'createObject' - NULL fields\n");
        exit(0);
    }

    JSON_NODE* first = createJsonNodeFromField(fields[0]);

    for(int i = 1; i<countFields; i++)
        addToJsonToNPos(&first, createJsonNodeFromField(fields[i]), -1);

    return first;
}
void deleteObject(JSON_NODE** object) {

    if (object == NULL || *object == NULL) {
        printf("Error in 'deleteObject' - NULL object\n");
        exit(0);
    }

    removeJson(object);
}

bool hasObjectField(JSON_NODE* object, char* fieldName) {

    if(object == NULL) {
        printf("Error in 'hasObjectField' - NULL object\n");
        exit(0);
    }
    if(fieldName == NULL) {
        printf("Error in 'hasObjectField' - NULL fieldName\n");
        exit(0);
    }

    JSON_NODE* temp = object;

    while(temp != NULL) {
        if(compareString(temp->objectFieldName, fieldName) == 0)
            return true;
        temp = temp->next;
    }

    return false;
}
JSON_NODE* findByFieldName(JSON_NODE* object, char* fieldName) {

    if(object == NULL) {
        printf("Error in 'findByFieldName' - NULL object\n");
        exit(0);
    }
    if(fieldName == NULL) {
        printf("Error in 'findByFieldName' - NULL fieldName\n");
        exit(0);
    }

    JSON_NODE* temp = object;

    while(temp != NULL) {
        if(compareString(temp->objectFieldName, fieldName) == 0)
            return temp;
        temp = temp->next;
    }

    return NULL;
}

void addToObjectToNPlace(JSON_NODE** objectNode, JSON_NODE* value, int position) {

    if(objectNode == NULL) {
        printf("Error in 'addToObjectToNPlace' - NULL objectNode");
        exit(0);
    }
    if(value == NULL) {
        printf("Error in 'addToObjectToNPlace - NULL value\n");
        exit(0);
    }
    if(position < -1 || position > jsonLength(*objectNode)) {
        printf("Error in 'addToObjectToNPlace' - wrong position\n");
        exit(0);
    }

    if(value->objectFieldName != NULL)
        addToJsonToNPos(objectNode, value, position);
}
void removeFromObject(JSON_NODE** objectNode, char* fieldName) {

    if(objectNode == NULL || *objectNode == NULL) {
        printf("Error in 'removeFromObject' - NULL objectNode\n");
        exit(0);
    }
    if(fieldName == NULL) {
        printf("Error in 'removeFromObject' - NULL fieldName\n");
        exit(0);
    }

    JSON_NODE* temp = findByFieldName(*objectNode, fieldName);
    removeJsonNode(&temp);
}

static void printField(JSON_NODE* jsonNode, int tabulation);

static void printJsonStructuredTabs(JSON_NODE* json, int tabulation) {

    if(json == NULL) {
        printf("Error in 'printJsonStructuredTabs' - NULL json\n");
        exit(0);
    }

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

    printf("}");
}

static void printArrayValue(JSON_NODE* jsonNode, int tabulation) {

    if(jsonNode == NULL) {
        printf("Error in 'printArrayValue' - NULL jsonNode\n");
        exit(0);
    }

    switch(jsonNode->type) {

        case boolean:
            if(jsonNode->boolVal == true)
                printf("\"true\"");
            else
                printf("\"false\"");
            break;
        case number:
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

    if(jsonNode == NULL) {
        printf("Error in 'printField' - NULL jsonNode\n");
        exit(0);
    }

    switch(jsonNode->type) {

        case boolean:
            if(jsonNode->boolVal == true)
                printf("\"%s\":\"true\"", jsonNode->objectFieldName);
            else
                printf("\"%s\":\"false\"", jsonNode->objectFieldName);
            break;
        case number:
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

    if(json == NULL) {
        printf("Error in 'printJsonStructured' - NULL json\n");
        exit(0);
    }
    
    printJsonStructuredTabs(json, 1);
}
void printJsonUnstructured(JSON_NODE* json) {

    if(json == NULL) {
        printf("Error in 'printJsonUnstructured' - NULL json\n");
        exit(0);
    }

    printf("%s\n", parseJsonToString(json));
}

static void addString(char** to, char* from) {

    if(to == NULL || *to == NULL) {
        printf("Error in 'addString' - NULL to\n");
        exit(0);
    }
    if(from == NULL) {
        printf("Error in 'addString' - NULL from\n");
        exit(0);
    }

    *to = realloc(*to, stringLength(*to) + stringLength(from) + 1);

    int i = stringLength(*to);

    for(int j = 0; j<stringLength(from); j++, i++)
        (*to)[i] = from[j];

    (*to)[i] = '\0';
}

static void addArrayValue(JSON_NODE* jsonNode, char** result) {

    if(jsonNode == NULL) {
        printf("Error in 'addArrayValue' - NULL jsonNode\n");
        exit(0);
    }
    if(result == NULL || *result == NULL) {
        printf("Error in 'addArrayValue' - NULL result\n");
        exit(0);
    }

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
            for(JSON_NODE* temp = jsonNode->current; temp != NULL; temp = temp->next) {

                addArrayValue(jsonNode, result);

                if(temp->next != NULL)
                    addString(result, ",");
            }

            addString(result, "]");
    }

}

static void addField(JSON_NODE* jsonNode, char** result) {

    if(jsonNode == NULL) {
        printf("Error in 'addField' - NULL jsonNode\n");
        exit(0);
    }
    if(result == NULL || *result == NULL) {
        printf("Error in 'addField' - NULL result\n");
        exit(0);
    }

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
            for(JSON_NODE* temp = jsonNode->current; temp != NULL; temp = temp->next) {

                addArrayValue(temp, result);

                if(temp->next != NULL)
                    addString(result, ",");
            }

            addString(result, "]");
    }
}

char* parseJsonToString(JSON_NODE* json) {

    if(json == NULL) {
        printf("Error in 'parseJsonToString' - NULL json\n");
        exit(0);
    }

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

static char* parsePart(char* str, int* i) {

    if(str == NULL) {
        printf("Error in 'parsePart' - NULL string\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parsePart' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parsePart' - wrong index\n");
        exit(0);
    }

    (*i)++;
    int j = *i;
    while(j < stringLength(str) && str[j] != '\"')
        j++;

    if(j == stringLength(str)) {
        printf("Error in 'parsePart' - j out of borders\n");
        exit(0);
    }

    char* result = (char*)malloc(j - *i);

    int a = 0;
    for(a = 0; *i < stringLength(str) && *i<j; a++, (*i)++)
        result[a] = str[*i];

    result[a] = '\0';

    return result;
}
static bool isBool(char* str, int i) {

    if(str == NULL) {
        printf("Error in 'containsBool' - NULL string\n");
        exit(0);
    }
    if(i < 0 || i > stringLength(str)) {
        printf("Error in 'containsBool' - wrong index\n");
        exit(0);
    }

    if(i + 4 > stringLength(str)) {

        if (str[i] == 't' && str[i + 1] == 'r' && str[i + 2] == 'u' && str[i + 3] == 'e')
            return true;

        if (str[i] == 'f' && str[i + 1] == 'a' && str[i + 2] == 'l' && str[i + 3] == 's' && str[i + 4] == 'e')
            return true;
    }

    return false;
}
static bool isNumber(char symb) {

    if(symb == '-' || (symb >= '0' && symb <= '9'))
        return true;
    return false;
}
static bool isString(char symb) {

    return symb == '\"';
}
static bool isArray(char symb) {

    return symb == '[';
}
static bool isObject(char symb) {

    return symb == '{';
}

static void parseBool(JSON_NODE** result, char* fieldName, char* str, int* i) {

    if(result == NULL) {
        printf("Error in 'parseBool' - NULL result\n");
        exit(0);
    }
    if(fieldName == NULL ) {
        printf("Error in 'parseBool' - NULL fieldName\n");
        exit(0);
    }
    if(str == NULL) {
        printf("Error in 'parseBool' - NULL str\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parseBool' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parseBool' - wrong index\n");
        exit(0);
    }

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
static void parseString(JSON_NODE** result, char* fieldName, char* str, int* i) {

    if(result == NULL) {
        printf("Error in 'parseString' - NULL result\n");
        exit(0);
    }
    if(fieldName == NULL ) {
        printf("Error in 'parseString' - NULL fieldName\n");
        exit(0);
    }
    if(str == NULL) {
        printf("Error in 'parseString' - NULL str\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parseString' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parseString' - wrong index\n");
        exit(0);
    }

    addToJsonToNPos(result, createStringJsonNode(parsePart(str, i), fieldName), -1);
}
static void parseNumber(JSON_NODE** result, char* fieldName, char* str, int* i) {

    if(result == NULL) {
        printf("Error in 'parseNumber' - NULL result\n");
        exit(0);
    }
    if(fieldName == NULL ) {
        printf("Error in 'parseNumber' - NULL fieldName\n");
        exit(0);
    }
    if(str == NULL) {
        printf("Error in 'parseNumber' - NULL str\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parseNumber' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parseNumber' - wrong index\n");
        exit(0);
    }    

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

static void parseObject(JSON_NODE** result, char* fieldName, char* str, int* i);

static void parseArray(JSON_NODE** result, char* fieldName, char* str, int* i) {

    if(result == NULL) {
        printf("Error in 'parseArray' - NULL result\n");
        exit(0);
    }
    if(fieldName == NULL ) {
        printf("Error in 'parseArray' - NULL fieldName\n");
        exit(0);
    }
    if(str == NULL) {
        printf("Error in 'parseArray' - NULL str\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parseArray' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parseArray' - wrong index\n");
        exit(0);
    }

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
static void parseObject(JSON_NODE** result, char* fieldName, char* str, int* i) {

    if(result == NULL) {
        printf("Error in 'parseObject' - NULL result\n");
        exit(0);
    }
    if(fieldName == NULL ) {
        printf("Error in 'parseObject' - NULL fieldName\n");
        exit(0);
    }
    if(str == NULL) {
        printf("Error in 'parseObject' - NULL str\n");
        exit(0);
    }
    if(i == NULL) {
        printf("Error in 'parseObject' - NULL index\n");
        exit(0);
    }
    if(*i < 0 || *i > stringLength(str)) {
        printf("Error in 'parseObject' - wrong index\n");
        exit(0);
    }    

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

JSON_NODE* parseStringToJson(char* str) {

    if(str == NULL) {
        printf("Error in 'parseStringToJson' - NULL string\n");
        exit(0);
    }

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