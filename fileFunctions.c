#include "fileFunctions.h"

FILE* openFile(const char* fileName, const char* mode) {

    FILE* f = fopen(fileName, mode);
    if(f == NULL) {
        printf("Error while open file %s.\n", fileName);
        exit(0);
    }

    return f;
}

bool ifFileEmpty(FILE* f) {

    long currentPos = ftell(f);

    fseek(f, 0, SEEK_END);
    bool result = ftell(f) == currentPos;

    fseek(f, currentPos, SEEK_SET);

    return result;
}

void addStringToFile(const char* fileName, const char* data) {

    FILE* f = openFile(fileName, "a+");

    if(ifFileEmpty(f))
        fprintf(f, "%s", data);
    else
        fprintf(f, "\n%s", data);

    fclose(f);
}

void showContent(const char* fileName) {

    FILE* f = openFile(fileName, "r");

    struct flock fl;
    fl.l_type = F_RDLCK;
    fl.l_len = 0;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1) {
        printf("Error while making block.\n");
        fclose(f);
        exit(0);
    }

    if(ifFileEmpty(f))
        printf("No data.\n");
    else {
        while (!feof(f)) {
            char *temp = (char *) malloc(256);

            fgets(temp, 256, f);
            temp[stringLength(temp) - 1] = '\0';

            printJsonStructured(parseStringToJson(temp));

            free(temp);
        }
    }

    fl.l_type = F_UNLCK;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1) {
        printf("Error while breaking block.\n");
        fclose(f);
        exit(0);
    }

    fclose(f);
}

static int compareByField(const char* str1, const char* str2, const char* fieldName) {

    int i = findSubstring(str1, fieldName), j = findSubstring(str2, fieldName);
    while(str1[i++] != '"');
    i+=2;
    while(str2[j++] != '"');
    j+=2;

    return compareString(copyStringFromIToSymbol(str1, &i, '"'), copyStringFromIToSymbol(str2, &j, '"'));
}

static void quicksort(char*** arr, int left, int right, const char* fieldName) {

    if (left < right) {
        int middleIndex = (left + right) / 2;
        char *middle = (*arr)[middleIndex];
        int i = left;
        int j = right;

        while (i <= j) {
            while (compareByField((*arr)[i], middle, fieldName) < 0)
                i++;
            while (compareByField((*arr)[j], middle, fieldName) > 0)
                j--;
            if (i <= j) {
                char *temp;
                copyString(&temp, (*arr)[i]);
                copyString(&(*arr)[i], (*arr)[j]);
                copyString(&(*arr)[j], temp);
                free(temp);
                i++;
                j--;
            }
        }

        quicksort(arr, left, j, fieldName);
        quicksort(arr, i, right, fieldName);
    }
}

void sortContent(const char* fileName, const char* field) {

    FILE* f = openFile(fileName, "r+");

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_len = 0;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1) {
        printf("Error while making block.\n");
        fclose(f);
        exit(0);
    }

    int count = 1;
    while(!feof(f)) {

        while(fgetc(f) != '\n');
        count++;
    }
    fseek(f, 0, SEEK_SET);

    char** strings = (char**)malloc(count * sizeof(char*));

    for(int i = 0; i<count; i++) {

        strings[i] = (char*)malloc(256);
        fgets(strings[i], 256, f);
    }

    quicksort(&strings, 0, count - 1, field);

    fseek(f, 0, SEEK_SET);

    for(int i = 0; i<count; i++)
        fputs(strings[i], f);

    for(int i = 0; i<count; i++)
        free(strings[i]);
    free(strings);

    fl.l_type = F_UNLCK;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1) {
        printf("Error while breaking block.\n");
        fclose(f);
        exit(0);
    }

    fclose(f);
}

void clearFile(const char* fileName) {

    FILE* f = openFile(fileName, "w");
    fclose(f);
}

void readToQueue(NODE** queue, const char* fileName) {

    FILE* f = openFile(fileName, "r");

    if(!ifFileEmpty(f)) {
        while (!feof(f)) {

            char *temp = (char *) malloc(256);
            fgets(temp, 256, f);
            temp[stringLength(temp) - 1] = '\0';

            push(queue, temp);
            free(temp);
        }
    }

    fclose(f);
}

void writeFromQueue(NODE* queue, const char* fileName) {

    FILE* f = openFile(fileName, "w");

    while(queue != NULL) {

        if(queue->next != NULL)
            fprintf(f, "%s\n", queue->data);
        else
            fprintf(f, "%s", queue->data);

        pop(&queue);
    }

    fclose(f);
}