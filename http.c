#include "http.h"

// Инициализация сокетного соединения клиента.
int initClient(const char* address, int portNumber) {

    pthread_self() == thread1 ? sPush(&checkStack1, "initClient") : sPush(&checkStack2, "initClient");

    if(address == NULL)
        return *(int*)handleError("address is NULL.", true, NULL, 6, NULL, 0, NULL);

    struct hostent* server;                                                                                  // Структура для представления информации о хосте.
    struct sockaddr_in addr;                                                                                 // Структура для представления сетевого адреса протокола IPv4
    if(portNumber == -1)                                                                                     // Обработка номера порта.
        portNumber = 8080;

    server = gethostbyname(address);                                                                   // Получение информации о хосте по его имени.

    addr.sin_family = AF_INET;                                                                               // Установка параметров сетевого адреса.
    addr.sin_port = htons(portNumber);
    bcopy((char*)server->h_addr_list[0], (char*)&addr.sin_addr.s_addr, server->h_length);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);                                           // Инициализация сокета для клиента.
    if(sfd == -1)
        return *(int*) handleError("error while create socket.", true, NULL, 6, NULL, 1, (int[]){sfd});

    while(connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {                         // Ожидание соединения с сервером.

        if(errno == ENOENT) {                                                                                // Проверка, что хост или порт просто не существует или не отвечает.
            sleep(1);
            continue;
        } else
            return *(int*) handleError("error while connect to server.", true, NULL, 6, NULL, 1, (int[]){sfd});
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return sfd;
}

// Инициализация сервера.
int initServer(int portNumber) {

    pthread_self() == thread1 ? sPush(&checkStack1, "initServer") : sPush(&checkStack2, "initServer");

    if(portNumber == -1)                                                                                     // Обработка номера порта.
        portNumber = 8080;

    struct sockaddr_in addr;                                                                                 // Структура для представления сетевого адреса протокола IPv4.
    addr.sin_family = AF_INET;                                                                               // Установка параметров сетевого адреса.
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = INADDR_ANY;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);                                          // Инициализация сокета.
    if(sfd == -1)
        return *(int*)handleError("error while create socket.", true, NULL, 6, NULL, 1, (int[]){sfd});

    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)      // Установка параметров сокета.
        return *(int*)handleError("error while set socket options.", true, NULL, 6, NULL, 1, (int[]){sfd});

    if(bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1)                                 // Привязка сокета к порту.
        return *(int*)handleError("error while bind socket.", true, NULL, 6, NULL, 1, (int[]){sfd});

    if(listen(sfd, 50) == -1)                                                                        // Сокет готов принимать клиентов.
        return *(int*)handleError("error while listen.", true, NULL, 6, NULL, 1, (int[]){sfd});

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return sfd;
}

// Инициализация HTTP пустого запроса.
HTTP_REQUEST* requestInit() {

    HTTP_REQUEST* request = REQUEST_MALLOC;

    request->content = NULL;                                                                               // Инициализация указателей пустыми значениями.
    request->headers = NULL;
    request->url = NULL;
    request->port = -1;

    return request;
}

// Инициализация HTTP запроса с параметрами.
HTTP_REQUEST* requestInitWithParams(int requestType, const char* url, HEADER* headers, const char* content, int portNumber) {

    pthread_self() == thread1 ? sPush(&checkStack1, "requestInitWithParams") : sPush(&checkStack2, "requestInitWithParams");

    if(url == NULL)
        return (HTTP_REQUEST*)handleError("url is NULL.", true, NULL, 5, NULL, 0, NULL);

    HTTP_REQUEST* request = REQUEST_MALLOC;

    request->requestType = requestType;                                                                   // Указание значений полям запроса.
    copyString(&request->url, url);
    if(content != NULL)
        copyString(&request->content, content);
    else
        request->content = NULL;
    request->headers = headers;
    request->port = portNumber;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return request;
}

// Очистка HTTP запроса.
bool requestFree(HTTP_REQUEST** request) {

    pthread_self() == thread1 ? sPush(&checkStack1, "requestFree") : sPush(&checkStack2, "requestFree");

    if(checkDoublePointer((const void**)request) == false)
        return *(bool*)handleError("request is NULL.", true, NULL, 1, NULL, 0, NULL);

    headersFree(&(*request)->headers);                                                                  // Очистка всех указателей.
    free((*request)->content);
    free((*request)->url);
    free(*request);

    *request = NULL;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Инициализация пустого HTTP ответа.
HTTP_RESPONSE* responseInit() {

    HTTP_RESPONSE* response = RESPONSE_MALLOC;

    response->headers = NULL;                                                                         // Инициализация указателей пустыми значениями.
    response->content = NULL;
    response->responseStatus = NULL;

    return response;
}

// Инициализация HTTP ответа с параметрами.
HTTP_RESPONSE* responseInitWithParams(const char* responseStatus, HEADER* headers, const char* content) {

    pthread_self() == thread1 ? sPush(&checkStack1, "responseInitWithParams") : sPush(&checkStack2, "responseInitWithParams");

    if(responseStatus == NULL)
        return (HTTP_RESPONSE*)handleError("responseStatus is NULL.", true, NULL, 4, NULL, 0, NULL);

    HTTP_RESPONSE* response = RESPONSE_MALLOC;

    response->headers = headers;                                                                      // Указание значений полям.
    if(content != NULL)
        copyString(&response->content, content);
    else
        response->content = NULL;
    copyString(&response->responseStatus, responseStatus);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return response;
}

// Очистка HTTP ответа.
bool responseFree(HTTP_RESPONSE** response) {

    pthread_self() == thread1 ? sPush(&checkStack1, "responseFree") : sPush(&checkStack2, "responseFree");

    if(checkDoublePointer((const void**)response) == false)
        return *(bool*)handleError("response is NULL.", true, NULL, 1, NULL, 0, NULL);

    headersFree(&(*response)->headers);                                                              // Очистка всех указателей.
    free((*response)->content);
    free((*response)->responseStatus);

    *response = NULL;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Парсинг строки название_заголовка:значений
static char** parseNameValueString(const char* nameValue) {

    char **result = (char **) malloc(2 * sizeof(char *));
    int i = 0;

    result[0] = copyStringFromIToSymbol(nameValue, &i, ':');                                // Разделение строки на две части.
    i += 2;
    result[1] = copyStringFromIToSymbol(nameValue, &i, '\0');

    return result;
}

// Инициализация заголовков.
HEADER* initHeaders(int count, const char** nameValues) {

    pthread_self() == thread1 ? sPush(&checkStack1, "initHeaders") : sPush(&checkStack2, "initHeaders");

    if(checkNumber(count, 1, -1) == false || checkArray((const void**)nameValues, count) == false)
        return (HEADER*) handleError("nameValues is NULL or count has wrong value.", true, NULL, 3, NULL, 0, NULL);

    HEADER* headers = NULL;
    for(int i = 0; i<count; i++)                                                                    // Обход всех заголовков.
        addToHeaders(&headers, nameValues[i]);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return headers;
}

// Добавление значения в заголовки.
static void addToHeaders(HEADER** headers, const char* nameValue) {

    HEADER* tempHeader = *headers;

    if(tempHeader == NULL) {
        *headers = HEADER_MALLOC;                                                                   // Добавление в начало, если ещё нет заголовков.
        (*headers)->next = NULL;
        (*headers)->prev = NULL;
        tempHeader = *headers;
    } else {
        while (tempHeader->next != NULL)                                                            // Переход в конец.
            tempHeader = tempHeader->next;

        tempHeader->next = HEADER_MALLOC;                                                           // Добавление в конец.
        tempHeader->next->next = NULL;
        tempHeader->next->prev = tempHeader;
        tempHeader = tempHeader->next;
    }

    char** result = parseNameValueString(nameValue);                                                // Добавление названия заголовка и значения.
    copyString(&tempHeader->name, result[0]);
    copyString(&tempHeader->value, result[1]);

    free(result[0]);
    free(result[1]);
    free(result);
}

// Поиск заголовка по названию.
HEADER* findHeader(HEADER* headers, const char* name) {

    pthread_self() == thread1 ? sPush(&checkStack1, "findHeader") : sPush(&checkStack2, "findHeader");

    if(headers == NULL || name == NULL)
        return (HEADER*) handleError("headers or name is NULL.", true, NULL, 3, NULL, 0, NULL);

    HEADER* result = headers;

    while(result != NULL && compareString(result->name, name) != 0)                        // Обход всех и сравнение.
        result = result->next;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Освобождение всех заголовков.
static void headersFree(HEADER** headers) {

    while(*headers != NULL) {                                                                       // Обход всех и освобождение.

        free((*headers)->value);
        free((*headers)->name);

        HEADER* temp = *headers;
        *headers = (*headers)->next;
        free(temp);
    }
}

// Установить тип HTTP запроса.
bool setHttpRequestType(HTTP_REQUEST** request, int type) {

    pthread_self() == thread1 ? sPush(&checkStack1, "setHttpRequestType") : sPush(&checkStack2, "setHttpRequestType");

    if(checkDoublePointer((const void**)request) == false || checkNumber(type, 0, 3) == false)
        return *(bool*)handleError("request is NULL or type has wrong value.", true, NULL, 1, NULL, 0, NULL);

    (*request)->requestType = type;                                                                // Установка типа.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Установка юрл HTTP запроса.
bool setHttpRequestUrl(HTTP_REQUEST** request, const char* url) {

    pthread_self() == thread1 ? sPush(&checkStack1, "setHttpRequestUrl") : sPush(&checkStack2, "setHttpRequestUrl");

    if(checkDoublePointer((const void**)request) == false || url == NULL)
        return *(bool*)handleError("request is NULL or url is NULL.", true, NULL, 1, NULL, 0, NULL);

    char* host, *address;
    int i = 0, iLen = stringLength(url);
    for(;i < iLen && url[i] != '/' && url[i] != ':'; i++);                                          // Пропуск http://, если есть.

    if(i < iLen && url[i+1] == '/')
        i+=2;
    else
        i = 0;

    bool hasPort = false;
    for(int j = i; url[j] != '/'; j++)                                                              // Проверка на наличие порта.
        if(url[j] == ':') {
            hasPort = true;
            break;
        }

    if(hasPort == true) {

        host = copyStringFromIToSymbol(url, &i, ':');
        i++;
        (*request)->port = (int)stringToDouble(copyStringFromIToSymbol(url, &i, '/')); // Считывание порта, если есть.
    }
    else
        host = copyStringFromIToSymbol(url, &i, '/');

    if(i < iLen)
        address = copyStringFromIToSymbol(url, &i, '\0');                                  // Считывание адреса.
    else
        copyString(&address, "/");

    char* header = (char*)malloc(7 + stringLength(host));
    sprintf(header, "Host: %s", host);

    addHttpRequestHeader(request, initHeaders(1, (const char*[]){header}));  // Установка хоста.

    copyString(&(*request)->url, address);                                                         // Установка адреса.

    free(header);
    free(address);
    free(host);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Добавление заголовка в HTTP запрос.
bool addHttpRequestHeader(HTTP_REQUEST** request, HEADER* httpHeader) {

    pthread_self() == thread1 ? sPush(&checkStack1, "addHttpRequestHeader") : sPush(&checkStack2, "addHttpRequestHeader");

    if(checkDoublePointer((const void**)request) == false || httpHeader == NULL || IS_HEADER_ERROR(*httpHeader))
        return *(bool*)handleError("request or httpHeader is NULL.", true, NULL, 1, NULL, 0, NULL);

    if((*request)->headers == NULL)                                                                  // Добавление заголовка в начало или в конец.
        (*request)->headers = httpHeader;
    else {
        HEADER* tempHeader = (*request)->headers;

        while(tempHeader->next != NULL)
            tempHeader = tempHeader->next;

        tempHeader->next = httpHeader;
        httpHeader->prev = tempHeader;
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Установка данных HTTP запроса.
bool setHttpRequestData(HTTP_REQUEST** request, const char* data) {

    pthread_self() == thread1 ? sPush(&checkStack1, "setHttpRequestData") : sPush(&checkStack2, "setHttpRequestData");

    if(checkDoublePointer((const void**)request) == false || data == NULL)
        return *(bool*)handleError("request or data is NULL.", true, NULL, 1, NULL, 0, NULL);

    copyString(&(*request)->content, data);                                                 // Копирование данных.

    char* header = (char*)malloc(17 + findNumberLen(stringLength(data)));
    sprintf(header, "Content-Length: %d", stringLength(data));

    addHttpRequestHeader(request, initHeaders(1, (const char*[]){header})); // Добавление заголовка с длиной данных.

    free(header);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Конвертация HTTP запроса в строку.
char* convertHttpRequestToString(HTTP_REQUEST* request) {

    pthread_self() == thread1 ? sPush(&checkStack1, "convertHttpRequestToString") : sPush(&checkStack2, "convertHttpRequestToString");

    if(request == NULL)
        return handleError("request is NULL.", true, NULL, 0, NULL, 0, NULL);

    char *result = (char*) malloc(1);
    result[0] = '\0';

    char *temp = (char *) malloc(256);
    switch (request->requestType) {                                                                  // Обработка первой строки запроса.
        case put_request:
            sprintf(temp, "PUT %s %s\r\n", request->url, HTTP_PROTOCOL_VERSION);
            break;
        case get_request:
            sprintf(temp, "GET %s %s\r\n", request->url, HTTP_PROTOCOL_VERSION);
            break;
        case post_request:
            sprintf(temp, "POST %s %s\r\n", request->url, HTTP_PROTOCOL_VERSION);
            break;
        case delete_request:
            sprintf(temp, "DELETE %s %s\r\n", request->url, HTTP_PROTOCOL_VERSION);
            break;
    }
    addString(&result, temp);
    free(temp);

    for (HEADER *header = request->headers; header != NULL; header = header->next) {                // Обработка заголовков запроса.
        temp = (char *) malloc(256);

        sprintf(temp, "%s: %s\r\n", header->name, header->value);
        addString(&result, temp);

        free(temp);
    }
    addString(&result, "\r\n");

    if (request->content != NULL)
        addString(&result, request->content);                                              // Обработка содержимого запроса.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Конвертировать строку в HTTP запрос.
HTTP_REQUEST* convertHttpRequestToObject(const char* request) {

    pthread_self() == thread1 ? sPush(&checkStack1, "convertHttpRequestToObject") : sPush(&checkStack2, "convertHttpRequestToObject");

    if(request == NULL)
        return (HTTP_REQUEST*)handleError("request or data is NULL.", true, NULL, 5, NULL, 0, NULL);

    HTTP_REQUEST* result = REQUEST_MALLOC;

    int i = 0;
    switch (request[0]) {                                                                           // Получение типа запроса.
        case 'G':
            result->requestType = get_request;
            break;
        case 'D':
            result->requestType = delete_request;
            break;
        case 'P':
            request[1] == 'O' ? (result->requestType = post_request) : (result->requestType = put_request);
            break;
    }

    while(request[i++] != ' ');
    char* temp = copyStringFromIToSymbol(request, &i, ' ');
    copyString(&result->url, temp);                                                       // Получение юрл запроса.
    free(temp);

    while(request[i++] != '\n');

    HEADER* headers = NULL;
    while(i < stringLength(request) && request[i] != '\r' && request[i] != '\0') {           // Получение заголовков запроса.

        temp = copyStringFromIToSymbol(request, &i, '\r');
        addToHeaders(&headers, temp);
        free(temp);
        if(request[i] != '\0')
            i+=2;
    }
    result->headers = headers;

    i+=2;
    if(i < stringLength(request)) {
        temp = copyStringFromIToSymbol(request, &i, '\0');
        copyString(&result->content, temp);                                              // Получение содержимого запроса, если есть.
        free(temp);
    }
    else
        result->content = NULL;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Проверка запроса на наличие необходимых данных.
static bool checkHttpRequest(HTTP_REQUEST* request) {

    if(request->url == NULL)
        return false;
    if(request->headers == NULL)
        return false;

    if(findHeader(request->headers, "Host") == NULL || findHeader(request->headers, "User-Agent") == NULL)  // Поиск нужных заголовков.
        return false;
    if(request->requestType == get_request) {
        if(findHeader(request->headers, "Accept") == NULL)
            return false;
    } else {
        if(findHeader(request->headers, "Content-Type") == NULL || findHeader(request->headers, "Content-Length") == NULL)
            return false;
    }

    return true;
}

// Отправка HTTP запроса.
HTTP_RESPONSE* sendHttpRequest(HTTP_REQUEST** request) {

    pthread_self() == thread1 ? sPush(&checkStack1, "sendHttpRequest") : sPush(&checkStack2, "sendHttpRequest");

    if(checkDoublePointer((const void**)request) == false)
        return (HTTP_RESPONSE*)handleError("request is NULL.", true, NULL, 4, NULL, 0, NULL);

    if (((*request)->content == NULL && (*request)->requestType != get_request) && ((*request)->headers == NULL || findHeader((*request)->headers, "Content-Length") == NULL))
        addHttpRequestHeader(request, initHeaders(1, (const char*[]){"Content-Length: 0"}));

    if (checkHttpRequest(*request) == false)
        return (HTTP_RESPONSE*)handleError("request hasn't got enough information.", true, NULL, 4, NULL, 0, NULL);

    int sfd = initClient(findHeader((*request)->headers, "Host")->value, (*request)->port);     // Инициализация клиента.
    if(sfd == -1)
        return (HTTP_RESPONSE*)handleError("error while initialise client.", true, NULL, 4, NULL, 0, NULL);

    char *httpRequestString = convertHttpRequestToString(*request);                                                     // Получение запроса в строковом представлении.
    char *httpResponseString = (char*)malloc(256);                                                                 // Выделение места под ответ.

    struct timeval timeout;                                                                                             // Структура для времени ожидания получения ответа.
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    fd_set readfds;                                                                                                     // Набор дескрипторов для select.
    FD_ZERO(&readfds);                                                                                                  // Инициализация нулевым значением набора.
    FD_SET(sfd, &readfds);                                                                                              // Добавление дескриптора клиента.

    if (send(sfd, httpRequestString, stringLength(httpRequestString) + 1, 0) <= 0)                    // Отправить запрос.
        return (HTTP_RESPONSE*)handleError("error while send request.", true, NULL, 4, NULL, 1, (int[]){sfd});

    int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);                                  // Ожидание ответа от сервера.
    if(ready <= 0)
        return (HTTP_RESPONSE*)handleError("error while wait for response.", true, NULL, 4, NULL, 1, (int[]){sfd});

    if (recv(sfd, httpResponseString, 256, 0) <= 0)                                                     // Приём ответа.
        return (HTTP_RESPONSE*)handleError("error while receive response from server.", true, NULL, 4, NULL, 1, (int[]){sfd});

    HTTP_RESPONSE *response = convertHttpResponseToObject(httpResponseString);                                  // Преобразование ответа в объект.

    free(httpRequestString);
    free(httpResponseString);
    close(sfd);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return response;
}

// Установка HTTP статуса для ответа.
bool setHttpResponseStatus(HTTP_RESPONSE** response, const char* status) {

    pthread_self() == thread1 ? sPush(&checkStack1, "setHttpResponseStatus") : sPush(&checkStack2, "setHttpResponseStatus");

    if(checkDoublePointer((const void**)response) == false || status == NULL)
        return *(bool*)handleError("response or status is NULL.", true, NULL, 1, NULL, 0, NULL);

    copyString(&(*response)->responseStatus, status);                                               // Установить тип.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Добавление заголовка в HTTP ответ.
bool addHttpResponseHeader(HTTP_RESPONSE** response, HEADER* httpHeader) {

    pthread_self() == thread1 ? sPush(&checkStack1, "addHttpResponseHeader") : sPush(&checkStack2, "addHttpResponseHeader");

    if(checkDoublePointer((const void**)response) == false || httpHeader == NULL)
        return *(bool*)handleError("response or httpHeader is NULL.", true, NULL, 1, NULL, 0, NULL);

    if((*response)->headers == NULL)                                                                        // Добавление заголовка в начало или конец.
        (*response)->headers = httpHeader;
    else {
        HEADER* tempHeader = (*response)->headers;

        while(tempHeader->next != NULL)
            tempHeader = tempHeader->next;

        tempHeader->next = httpHeader;
        httpHeader->prev = tempHeader;
    }

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Установка контента HTTP ответа.
bool setHttpResponseData(HTTP_RESPONSE** response, const char* data) {

    pthread_self() == thread1 ? sPush(&checkStack1, "setHttpResponseData") : sPush(&checkStack2, "setHttpResponseData");

    if(checkDoublePointer((const void**)response) == false || data == NULL)
        return *(bool*)handleError("response or data is NULL.", true, NULL, 1, NULL, 0, NULL);

    copyString(&(*response)->content, data);                                                        // Копирование содержимого.

    char* header = (char*)malloc(17 + findNumberLen(stringLength(data)));
    sprintf(header, "Content-Length: %d", stringLength(data));                               // Установка заголовка с длиной.

    addHttpResponseHeader(response, initHeaders(1, (const char*[]){header}));

    free(header);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}

// Конвертирование HTTP ответа в строку.
char* convertHttpResponseToString(HTTP_RESPONSE* response) {

    pthread_self() == thread1 ? sPush(&checkStack1, "convertHttpResponseToString") : sPush(&checkStack2, "convertHttpResponseToString");

    if(response == NULL)
        return handleError("response is NULL.", true, NULL, 0, NULL, 0, NULL);

    char *result = (char *) malloc(1);
    result[0] = '\0';

    char *temp = (char *) malloc(256);                                                                 // Обработка первой строки ответа.
    sprintf(temp, "%s %s\r\n", HTTP_PROTOCOL_VERSION, response->responseStatus);
    addString(&result, temp);
    free(temp);

    for (HEADER *header = response->headers; header != NULL; header = header->next) {                       // Обработка заголовков.
        temp = (char *) malloc(256);

        sprintf(temp, "%s: %s\r\n", header->name, header->value);
        addString(&result, temp);

        free(temp);
    }
    addString(&result, "\r\n");

    if (response->content != NULL)
        addString(&result, response->content);                                                    // Обработка содержимого ответа.

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Конвертирование строки в HTTP объект.
HTTP_RESPONSE* convertHttpResponseToObject(const char* response) {

    pthread_self() == thread1 ? sPush(&checkStack1, "convertHttpResponseToObject") : sPush(&checkStack2, "convertHttpResponseToObject");

    if(response == NULL)
        return (HTTP_RESPONSE*)handleError("response is NULL.", true, NULL, 4, NULL, 0, NULL);

    HTTP_RESPONSE * result = RESPONSE_MALLOC;
    int i = 0;

    while(response[i++] != ' ');

    char* temp = copyStringFromIToSymbol(response, &i, '\r');
    copyString(&result->responseStatus, temp);                                                   // Получение HTTP статуса.
    free(temp);
    i+=2;

    HEADER* headers = NULL;
    while(i < stringLength(response) && response[i] != '\r') {                                      // Получение заголовков.

        temp = copyStringFromIToSymbol(response, &i, '\r');
        addToHeaders(&headers, temp);
        free(temp);
        i+=2;
    }
    result->headers = headers;

    i+=2;
    if(i < stringLength(response)) {
        temp = copyStringFromIToSymbol(response, &i, '\0');
        copyString(&result->content, temp);                                                      // Получение содержимого ответа.
        free(temp);
    }
    else
        result->content = NULL;

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return result;
}

// Проверка HTTP ответа на наличие необходимых полей.
static bool checkHttpResponse(HTTP_RESPONSE* response) {

    if(response->responseStatus == NULL)
        return false;
    if(response->headers == NULL)
        return false;
    if(findHeader(response->headers, "Content-Length") == NULL)                                    // Поиск необходимых заголовков.
        return false;
    if(response->content != NULL)
        if(findHeader(response->headers, "Content-Type") == NULL)
            return false;

    return true;
}

// Получение HTTP запроса.
HTTP_REQUEST* getHttpRequest(int sfd) {

    pthread_self() == thread1 ? sPush(&checkStack1, "getHttpRequest") : sPush(&checkStack2, "getHttpRequest");

    char *httpRequestString = (char*)malloc(256);

    struct timeval timeout;                                                                                      // Структура для ожидания получения запроса.
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    fd_set readfds;                                                                                              // Набор дескрипторов для select.
    FD_ZERO(&readfds);
    FD_SET(sfd, &readfds);

    int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);                           // Ожидание, когда дескриптор будет готов принять запрос.
    if(ready <= 0)
        return (HTTP_REQUEST*)handleError("error while wait for server request.", true, NULL, 5, NULL, 0, NULL);

    if (recv(sfd, httpRequestString, 256, 0) <= 0) {                                            // Получение запроса.
        HTTP_RESPONSE* response = responseInitWithParams(HTTP_STATUS_500, initHeaders(1, (const char*[]){"Content-Length: 0"}), NULL);

        char* httpResponseString = convertHttpResponseToString(response);

        if (send(sfd, httpResponseString, stringLength(httpResponseString), 0) <= 0) {   // Обработка ситуации при ошибке.

            free(httpResponseString);
            responseFree(&response);
            return (HTTP_REQUEST*)handleError("error while send message to client about error while receive.", true, NULL, 5, NULL, 0, NULL);
        }

        free(httpResponseString);
        responseFree(&response);
    }

    HTTP_REQUEST* request = convertHttpRequestToObject(httpRequestString);
    free(httpRequestString);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return request;
}

// Послать HTTP ответ клиенту.
bool sendHttpResponse(int sfd, HTTP_RESPONSE** response) {

    pthread_self() == thread1 ? sPush(&checkStack1, "sendHttpResponse") : sPush(&checkStack2, "sendHttpResponse");

    if(checkDoublePointer((const void**)response) == false)
        return *(bool*)handleError("response is NULL.", true, NULL, 1, NULL, 0, NULL);

    if ((*response)->content == NULL && ((*response)->headers == NULL || findHeader((*response)->headers, "Content-Length") == NULL))                                                                       // Добавление не хватающего заголовка.
        addHttpResponseHeader(response, initHeaders(1, (const char*[]){"Content-Length: 0"}));

    if (checkHttpResponse(*response) == false)
        return *(bool*)handleError("not enough data in response.", true, NULL, 1, NULL, 0, NULL);

    char *httpResponseString = convertHttpResponseToString(*response);                                        // Преобразовать ответ в строку.

    if (send(sfd, httpResponseString, stringLength(httpResponseString), 0) <= 0)        // Отправить ответ.
        return *(bool*)handleError("error while send http response.", true, NULL, 1, NULL, 0, NULL);

    free(httpResponseString);

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return true;
}