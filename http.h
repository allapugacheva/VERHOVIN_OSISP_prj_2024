#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>
#include "json.h"

// Библиотека для работы с HTTP запросами.

#define HTTP_STATUS_200 "200 Ok"                                                  // Коды HTTP запросов.
#define HTTP_STATUS_201 "201 Created"
#define HTTP_STATUS_204 "204 No content"

#define HTTP_STATUS_400 "400 Bad request"
#define HTTP_STATUS_404 "404 Not found"

#define HTTP_STATUS_500 "500 Internal server error"

#define HTTP_PROTOCOL_VERSION "HTTP/1.1"                                          // Версия HTTP протокола.

#define REQUEST_MALLOC ((HTTP_REQUEST*)malloc(sizeof(HTTP_REQUEST)))              // Макросы для выделения места под запрос, ответ и заголовок.
#define RESPONSE_MALLOC ((HTTP_RESPONSE*)malloc(sizeof(HTTP_RESPONSE)))
#define HEADER_MALLOC ((HEADER*)malloc(sizeof(HEADER)))

typedef enum http_requests {                                                      // Типы HTTP запросов.
    get_request = 0,
    post_request,
    put_request,
    delete_request
} HTTP_REQUESTS;

typedef struct header {                                                           // Заголовок HTTP запроса/ответа.

    struct header* prev;
    struct header* next;
    char* name;                                                                   // Название заголовка.
    char* value;                                                                  // Значение.

} HEADER;

typedef struct http_request {                                                     // HTTP запрос.

    int requestType;                                                              // Тип запроса.
    int port;                                                                     // Порт, по которому происходит подключение.
    char* url;                                                                    // Юрл запроса.
    HEADER* headers;                                                              // Заголовки HTTP запроса.
    char* content;                                                                // Содержимое запроса.

} HTTP_REQUEST;

typedef struct http_response {                                                    // HTTP ответ.

    char* responseStatus;                                                         // HTTP статус ответа.
    HEADER* headers;                                                              // Заголовки HTTP ответа.
    char* content;                                                                // Содержимое ответа.

} HTTP_RESPONSE;

#define HEADER_ERROR &(HEADER){.value = "error"}
#define IS_HEADER_ERROR(header) (compareString((header).value, "error") == 0)

#define REQUEST_ERROR &(HTTP_REQUEST){.content = "error"}
#define IS_REQUEST_ERROR(request) ((request).content != NULL && compareString((request).content, "error") == 0)

#define RESPONSE_ERROR &(HTTP_RESPONSE){.content = "error"}
#define IS_RESPONSE_ERROR(response) ((response).content != NULL && compareString((response).content, "error") == 0)

int initClient(const char* address, int portNumber);                              // Инициализация клиента.
int initServer(int portNumber);                                                   // Инициализация сервера.

HTTP_REQUEST* requestInit();                                                      // Инициализация пустого HTTP запроса.
HTTP_REQUEST* requestInitWithParams(int requestType, const char* url, HEADER* headers, const char* content, int portNumber); // Инициализация HTTP запроса с параметрами.
bool requestFree(HTTP_REQUEST** request);                                         // Освобождение HTTP запроса.

HTTP_RESPONSE* responseInit();                                                    // Инициализация пустого ответа.
HTTP_RESPONSE* responseInitWithParams(const char* responseStatus, HEADER* headers, const char* content);  // Инициализация HTTP запроса с параметрами.
bool responseFree(HTTP_RESPONSE** response);                                      // Освобождение HTTP ответа.

HEADER* initHeaders(int count, const char** nameValues);                          // Инициализация заголовков.
HEADER* findHeader(HEADER* headers, const char* name);                            // Поиск заголовка по названию.

bool setHttpRequestType(HTTP_REQUEST** request, int type);                        // Установить тип HTTP запроса.
bool setHttpRequestUrl(HTTP_REQUEST** request, const char* url);                  // Установить юрл HTTP запроса.
bool addHttpRequestHeader(HTTP_REQUEST** request, HEADER* httpHeader);            // Добавить заголовок HTTP запроса.
bool setHttpRequestData(HTTP_REQUEST** request, const char* data);                // Установить содержимое HTTP запроса.

char* convertHttpRequestToString(HTTP_REQUEST* request);                          // Перевести HTTP запрос в строку.
HTTP_REQUEST* convertHttpRequestToObject(const char* request);                    // Перевести строку в HTTP запрос.

HTTP_RESPONSE* sendHttpRequest(HTTP_REQUEST** request);                           // Отправить HTTP запрос.

bool setHttpResponseStatus(HTTP_RESPONSE** response, const char* status);         // Установить статус HTTP ответа.
bool addHttpResponseHeader(HTTP_RESPONSE** response, HEADER* httpHeader);         // Добавить заголовок HTTP ответа.
bool setHttpResponseData(HTTP_RESPONSE** response, const char* data);             // Установить содержимое HTTP запроса.

char* convertHttpResponseToString(HTTP_RESPONSE* response);                       // Перевод HTTP ответа в строку.
HTTP_RESPONSE* convertHttpResponseToObject(const char* response);                 // Перевод строки в HTTP ответ.

HTTP_REQUEST* getHttpRequest(int sfd);                                            // Получить HTTP запрос.
bool sendHttpResponse(int sfd, HTTP_RESPONSE** response);                         // Отправить HTTP ответ.

//----------------------------------------------------------------------------------------------------------------------

static char** parseNameValueString(const char* nameValue);                        // Чтение названия заголовка и его содержимого.

static void addToHeaders(HEADER** headers, const char* nameValue);                // Добавление нового заголовка.
static void headersFree(HEADER** headers);                                        // Освобождение заголовков.

static bool checkHttpRequest(HTTP_REQUEST* request);                              // Проверить запрос на наличие необходимых полей.
static bool checkHttpResponse(HTTP_RESPONSE* response);                           // Проверка HTTP ответа на наличие необходимых полей.

// REQUEST:
// GET /api HTTP/1.1
// Host: example.com
// User-Agent: Raspberry pi zero w
// Content-Type: application/json
// Content-Length: 27
// **empty string**
// {"data":"data"}

// REQUEST WITHOUT DATA:
// GET /api HTTP/1.1
// Host: example.com
// User-Agent: Raspberry pi zero w
// Accept: application/json

// RESPONSE:
// HTTP/1.1 200 OK
// Content-Length: 27
// Content-Type: application/json
// **empty string**
// {"data":"data"}

// EMPTY RESPONSE:
// HTTP/1.1 204 No content
// Content-Length: 0
// **empty string**