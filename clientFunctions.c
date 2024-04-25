#include "client.h"

// Обработчик потока отправки сообщений на сервер.
void *sendHandler(void* arg) {

    sPush(&checkStack2, "sendHandler");

    while(continuing == true) {

        pthread_mutex_lock(&mutexQueue);                                                  // Блокировка мьютекса для работы с очередью.

        while (queue == NULL) {                                                                 // Ожидание заполнения очереди.

            if (continuing != true) {

                pthread_mutex_unlock(&mutexQueue);
                sPop(&checkStack2);
                return NULL;
            }
            pthread_cond_wait(&condQueue,&mutexQueue);                              // Условная переменная для передачи управления потоку заполнения очереди.
        }

        HTTP_REQUEST *request = requestInit();                                                  // Инициализация HTTP запроса.

        if (setHttpRequestUrl(&request, (char*)arg) == error || setHttpRequestType(&request, post_request) == error
            || addHttpRequestHeader(&request, initHeaders(2, (const char *[]) {"User-Agent: Raspberry pi zero w","Content-Type: application/json"})) == error
            || setHttpRequestData(&request, queue->data) == error) {

            handleError("error while form http request.", false, &continuing, 0, NULL, 0, NULL);
            pthread_mutex_unlock(&mutexQueue);
            break;
        }

        pthread_mutex_unlock(&mutexQueue);

        HTTP_RESPONSE *response = sendHttpRequest(&request);
        if (!IS_RESPONSE_ERROR(*response) && compareString(response->responseStatus, HTTP_STATUS_200) == 0) { // Проверка, что сообщение успешно отправлено.

            qPop(&queue);
            responseFree(&response);
        }
        else
            sleep(5);

        requestFree(&request);
    }

    sPop(&checkStack2);
}

// Обработчик потока считывания данных с одноплатного компьютера.
void *readHandler(void* arg) {

    sPush(&checkStack1, "readHandler");

    readToQueue(&queue, (char*)arg);                                                    // Прочитать данные из файла в очередь, если есть.
    if(queue != NULL)
        pthread_cond_signal(&condQueue);                                                  // Послать сигнал отправляющему потоку, если очередь не пуста.

    while(continuing == true) {

        RASPBERRY_SENSORS* tempData = readData();                                               // Чтение данных с датчиков.
        if(tempData == NULL) {
            handleError("error while read data from raspberry.", false, &continuing, 0, NULL, 0, NULL);
            break;
        }

        pthread_mutex_lock(&mutexQueue);                                                 // Блокировка мьютекса для работы с очередью.

        JSON_NODE* temp = createObject(4, (OBJECT_FIELD[]) {
                {"dateAndTime", string,  {.stringVal = tempData->dateTime},    stringLength(tempData->dateTime)},
                {"temperature", number,  {.numberVal = tempData->temperature}, 0},
                {"humidity",    number,  {.numberVal = tempData->humidity},    0},
                {"hasLoud",     boolean, {.boolVal = tempData->hasLoud},       0}
        });
        free(tempData);

//        JSON_NODE* temp = createObject(4, (OBJECT_FIELD[]) {                   // Создание объекта для пересылки.
//                {"dateAndTime", string,  {.stringVal = "today"}, stringLength("today")},
//                {"temperature", number,  {.numberVal = 11.1}, 0},
//                {"humidity",    number,  {.numberVal = 22.2}, 0},
//                {"hasLoud",     boolean, {.boolVal = false}, 0}
//        });

        if(IS_JSON_ERROR(*temp)) {                                                             // Проверка на ошибку создания.

            handleError("error while create object.", false, &continuing, 0, NULL, 0, NULL);
            pthread_mutex_unlock(&mutexQueue);
            break;
        }

        char* tempString = parseJsonToString(temp);                                       // Перевод в строку.
        if(tempString == NULL) {

            handleError("error while parse json object to string.", false, &continuing, 0, NULL, 0, NULL);
            removeJson(&temp);
            pthread_mutex_unlock(&mutexQueue);
            break;
        }

        qPush(&queue, tempString);                                                       // Запись данных в очередь.
        free(tempString);
        removeJson(&temp);

        if(queue->next == NULL)
            pthread_cond_signal(&condQueue);                                            // Послать сигнал отправляющему потоку, если очередь перестала быть пустой.

        pthread_mutex_unlock(&mutexQueue);
        sleep(10);
    }
    if(queue != NULL) {
        if (writeFromQueue(queue, (char*)arg) == error)                              // Запись данных, которые не были отправлены в файл, чтобы не потерять их.
            handleError("error while write data from queue to file.", false, &continuing, 0, NULL, 0, NULL);
    } else {
        if (clearFile((char*)arg) == error)
            handleError("error while clear file.", false, &continuing, 0, NULL, 0, NULL);
    }
    pthread_cond_signal(&condQueue);

    sPop(&checkStack1);
}

// Сигнал завершения работы программы.
void stop() {
    continuing = false;
}

// Сигнал завершения работы из-за ошибки.
void err() {
    continuing = error;
}