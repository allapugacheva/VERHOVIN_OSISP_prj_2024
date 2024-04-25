#include "fileFunctions.h"

// Проверка, что файл пуст.
static bool ifFileEmpty(FILE* f) {

    long currentPos = ftell(f);                                                         // Текущая позиция.

    fseek(f, 0, SEEK_END);                                                  // Переместиться в конец.
    bool result = ftell(f) == currentPos;                                               // Сравнить положение.

    fseek(f, currentPos, SEEK_SET);                                         // Вернуться назад.

    return result;
}

// Добавление строки в файл.
bool addStringToFile(const char* fileName, const char* data) {

    pthread_self() == thread1 ? sPush(&checkStack1, "addStringToFile") : sPush(&checkStack2, "addStringToFile");

    FILE* f = fopen(fileName, "a+");
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    fseek(f, 0, SEEK_END);

    if(ftell(f) == 0L)                                                                // Форматирование строки в зависимости от содержимого файла.
        fprintf(f, "%s", data);
    else
        fprintf(f, "\n%s", data);

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Вывод содержимого файла.
bool showContent(const char* fileName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "showContent") : sPush(&checkStack2, "showContent");

    FILE* f = fopen(fileName, "r");
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    struct flock fl;                                                                          // Структура для обеспечения конкурентного доступа к файлу.
    fl.l_type = F_RDLCK;                                                                      // Блокировка на чтение.
    fl.l_len = 0;
    fl.l_start = 0;                                                                           // Блокировать весь файл.
    fl.l_whence = SEEK_SET;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1)                                  // Установка блокировки файла.
        return *(bool*)handleError("error while make block.", true, NULL, 1, f, 0, NULL);

    if(ifFileEmpty(f) == true)
        printf("No data.\n");
    else {
        while (true) {                                    // Вывод содержимого.

            char* temp = (char*)malloc(256);

            if(fgets(temp, 256, f) == NULL) {
                free(temp);
                break;
            }

            if(!feof(f))
                temp[stringLength(temp) - 1] = '\0';

            JSON_NODE* tempJson = parseStringToJson(temp);                              // Преобразование строки в объект JSON.

            if(IS_JSON_ERROR(*tempJson)) {

                fl.l_type = F_UNLCK;                                                          // Разблокировка.
                if(fcntl(fileno(f), F_SETLKW, &fl) == -1)
                    return *(bool*)handleError("error while make unblock.", true, NULL, 1, f, 0, NULL);

                return *(bool*)handleError("wrong string format.", true, NULL, 1, f, 0, NULL);
            }

            printJsonStructured(tempJson);

            removeJson(&tempJson);
            free(temp);
        }
    }

    fl.l_type = F_UNLCK;                                                                     // Разблокировка.
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1)
        return *(bool*)handleError("error while make unblock.", true, NULL, 1, f, 0, NULL);

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Сравнение двух строк по указанному полю JSON объекта.
static int compareByField(const char* str1, const char* str2, const char* fieldName) {

    int i = findSubstring(str1, fieldName), j = findSubstring(str2, fieldName);    // Поиск нужных полей.
    if(i == -1 || j == -1)
        return 0;

    while(str1[i++] != '"');
    i+=1;
    while(str2[j++] != '"');
    j+=1;
                                                                                                               // Получение нужных полей.
    char* data1 = copyStringFromIToSymbol(str1, &i, ','), *data2 = copyStringFromIToSymbol(str2, &j, ',');
    int result = compareString(data1, data2);                                                        // Сравнение полей.

    free(data1);
    free(data2);

    return result;
}

// Быстрая сортировка массива строк.
static void quicksort(char*** arr, int left, int right, const char* fieldName) {

    if(left < right) {
        int middleIndex = (left + right) / 2;                                            // Средний элемент.
        char *middle = (*arr)[middleIndex];
        int i = left;                                                                    // Индексы для обхода.
        int j = right;

        while (i <= j) {
            while (compareByField((*arr)[i], middle, fieldName) < 0)           // Поиск первого в левой половине, кто стоит не так.
                i++;
            while (compareByField((*arr)[j], middle, fieldName) > 0)           // Поиск первого в правой половине, кто стоит не так.
                j--;
            if (i <= j) {
                char *temp = (*arr)[i];                                                 // Замена местами.
                (*arr)[i] = (*arr)[j];
                (*arr)[j] = temp;
                i++;
                j--;
            }
        }

        quicksort(arr, left, j, fieldName);                                        // Вызов для левой половины.
        quicksort(arr, i, right, fieldName);                                        // Вызов для правой половины.
    }
}

// Сортировка данных в файле.
bool sortContent(const char* fileName, const char* field) {

    pthread_self() == thread1 ? sPush(&checkStack1, "sortContent") : sPush(&checkStack2, "sortContent");

    FILE* f = fopen(fileName, "r+");
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    struct flock fl;                                                                    // Структура для конкурентного доступа к файлу.
    fl.l_type = F_WRLCK;                                                                // Блокировка на запись.
    fl.l_len = 0;
    fl.l_start = 0;                                                                     // Всего файла.
    fl.l_whence = SEEK_SET;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1)                           // Установка блокировки.
        return *(bool*)handleError("error while make block.", true, NULL, 1, f, 0, NULL);

    int realCount = 0, maxCount = 20;

    char** strings = (char**)malloc(maxCount*sizeof(char*));

    while(true) {

        if(realCount == maxCount) {
            maxCount *= 2;
            strings = (char**)realloc(strings, maxCount*sizeof(char*));
        }

        strings[realCount] = (char*)malloc(256);
        if(fgets(strings[realCount], 256, f) == NULL)                     // Чтение всех строк.
            break;
        if(!feof(f))
            strings[realCount][stringLength(strings[realCount]) - 1] = '\0';
        realCount++;
    }

    quicksort(&strings, 0, realCount - 1, field);              // Сортировка строк.

    fseek(f, 0, SEEK_SET);

    for(int i = 0; i<realCount; i++) {                                                // Запись в файл.
        if(i + 1 != realCount)
            fprintf(f, "%s\n", strings[i]);
        else
            fprintf(f, "%s", strings[i]);
    }

    for(int i = 0; i<realCount; i++)
        free(strings[i]);
    free(strings);

    fl.l_type = F_UNLCK;
    if(fcntl(fileno(f), F_SETLKW, &fl) == -1)                        // Разблокировка файла.
        return *(bool*)handleError("error while make unblock.", true, NULL, 1, f, 0, NULL);

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Очистка файла.
bool clearFile(const char* fileName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "clearFile") : sPush(&checkStack2, "clearFile");

    FILE* f = fopen(fileName, "w");                                          // Открыть файл с модификатором доступа создания нового.
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Запись данных из файла в очередь.
bool readToQueue(QNODE** queue, const char* fileName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "readToQueue") : sPush(&checkStack2, "readToQueue");

    FILE* f = fopen(fileName, "r");
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    if(ifFileEmpty(f) != true) {
        while (true) {

            char *temp = (char *)malloc(256);
            if(fgets(temp, 256, f) == NULL) {                                      // Чтение всех строк.
                free(temp);
                break;
            }
            if(!feof(f))
                temp[stringLength(temp) - 1] = '\0';

            qPush(queue, temp);                                               // Запись в очередь.
            free(temp);
        }
    }

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Запись данных из очереди в файл.
bool writeFromQueue(QNODE* queue, const char* fileName) {

    pthread_self() == thread1 ? sPush(&checkStack1, "writeFromQueue") : sPush(&checkStack2, "writeFromQueue");

    FILE* f = fopen(fileName, "w");
    if(f == NULL)
        return *(bool*)handleError("error while open file.", true, NULL, 1, f, 0, NULL);

    while(queue != NULL) {

        if(queue->next != NULL)                                                  // Запись всех данных в файл.
            fprintf(f, "%s\n", queue->data);
        else
            fprintf(f, "%s", queue->data);

        qPop(&queue);
    }

    fclose(f);
    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}