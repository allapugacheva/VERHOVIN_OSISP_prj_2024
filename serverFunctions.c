#include "server.h"

// Обработчик записи принятых сообщений в файл.
void* fillerHandler(void* arg) {

    sPush(&checkStack2, "fillerHandler");

    while(continuing == true) {

        pthread_mutex_lock(&mutexQueue);                            // Блокировка мьютекса для работы с очередью.

        while (queue == NULL) {                                           // Ожидание заполнения очереди сообщений.
            if(continuing != true) {                                      // Проверка, что не вызван сигнал завершения программы.
                pthread_mutex_unlock(&mutexQueue);
                sPop(&checkStack2);
                return NULL;
            }
            pthread_cond_wait(&condQueue, &mutexQueue);       // Отдаем мьютекс потоку чтения сообщений и ожидаем.
        }

        if(addStringToFile((char*)arg, queue->data) == true)      // Функция добавления данных в файл.
            qPop(&queue);                                                 // Удаление из очереди.
        else
            handleError("error while add data to file.", false, &continuing, 0, NULL, 0, NULL);

        pthread_mutex_unlock(&mutexQueue);                          // Разблокировка мьютекса.
    }
    sPop(&checkStack2);
}

// Обработчик приёма сообщений от клиента.
void *receiveHandler(void *arg) {

    sPush(&checkStack1, "receiveHandler");

    int sfd = initServer(*(int*)arg);                            // Инициализация сервера.
    if(sfd == -1)
       handleError("error while initialise server.", false, &continuing, 0, NULL, 0, NULL);

    struct timeval timeout;                                                // Структура для ожидания приёма соединения с клиентом.

    fd_set readfds;                                                        // Набор дескрипторов.

    while (continuing == true) {

        timeout.tv_sec = 5;                                                // 5 секунд.
        timeout.tv_usec = 0;

        FD_ZERO(&readfds);                                                 // Инициализация набора дескрипторов, устанавливает все биты в 0.
        FD_SET(sfd, &readfds);                                             // Добавление дескриптора сервера.
                                                                           // Ожидание, пока дескриптор станет доступен для принятия соединения с клиентом.
                                                                           // Параметры: количество файловых дескрипторов + 1, указатели на наборы дескрипторов для чтения,
        int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);  // записи (пустой), ошибок (пустой), максимальное время ожидания.
        if (ready <= 0)                                                    // Проверка, что дескриптор готов для работы.
            continue;

        int cfd = accept(sfd, NULL, 0);                   // Приём соединения с клиентом.
        if (cfd == -1) {                                                   // Проверка, что всё прошло успешно.
            handleError("error while client connection.", false, &continuing, 0, NULL, 1, (int[]){cfd});
            break;
        }
        HTTP_REQUEST *request = getHttpRequest(cfd);                  // Получение запроса от клиента.
        if(IS_REQUEST_ERROR(*request)) {
            handleError("error while get http request.", false, &continuing, 0, NULL, 1, (int[]){cfd});
            break;
        }
        pthread_mutex_lock(&mutexQueue);                             // Блокирование мьютекса для работы с очередью сообщений.
        qPush(&queue, request->content);                              // Добавление в очередь сообщений нового сообщения.
        if (queue->next == NULL)
            pthread_cond_signal(&condQueue);                         // Если было добавлено первое сообщение в очередь, то посылается сигнал обработчику сообщений о её заполнении.
        pthread_mutex_unlock(&mutexQueue);                          // Разблокирование мьютекса.

        requestFree(&request);                                            // Очистка запроса.
                                                                          // Формирование ответа клиенту.
        HTTP_RESPONSE *response = responseInitWithParams(HTTP_STATUS_200, NULL, NULL);
        if(sendHttpResponse(cfd, &response) == error || IS_RESPONSE_ERROR(*response))
            handleError("error while send response to client.", false, &continuing, 0, NULL, 0, NULL);
        else
            responseFree(&response);                                      // Очистка ответа.

        close(cfd);
    }

    close(sfd);
    pthread_cond_signal(&condQueue);                                // Отправка сигнала о завершении работы процессу по записи сообщений в файл.

    sPop(&checkStack1);
}

// Обработчик сигнала завершения работы.
void stop() {
    continuing = false;
}

// Обработчик сигнала ошибки.
void err() {
    continuing = error;
}