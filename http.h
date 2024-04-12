#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <sys/select.h>
#include "stringFunctions.h"
#include "checkingFunctions.h"


#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "json.h"


#define HTTP_STATUS_200 "200 Ok"
#define HTTP_STATUS_201 "201 Created"
#define HTTP_STATUS_204 "204 No content"

#define HTTP_STATUS_400 "400 Bad request"
#define HTTP_STATUS_404 "404 Not found"

#define HTTP_STATUS_500 "500 Internal server error"

#define HTTP_PROTOCOL_VERSION "HTTP/1.1"

#define REQUEST_MALLOC ((HTTP_REQUEST*)malloc(sizeof(HTTP_REQUEST)))
#define RESPONSE_MALLOC ((HTTP_RESPONSE*)malloc(sizeof(HTTP_RESPONSE)))
#define HEADER_MALLOC ((HEADER*)malloc(sizeof(HEADER)))

typedef enum http_requests {
    get_request = 0,
    post_request,
    put_request,
    delete_request
} HTTP_REQUESTS;

typedef struct header {

    struct header* prev;
    struct header* next;
    char* name;
    char* value;

} HEADER;

typedef struct http_request {

    int requestType;
    int port;
    char* url;
    HEADER* headers;
    char* content;

} HTTP_REQUEST;

typedef struct http_response {

    char* responseStatus;
    HEADER* headers;
    char* content;

} HTTP_RESPONSE;

int initClient(const char* address, int portNumber);
int initServer(int portNumber);

HTTP_REQUEST* requestInit();
HTTP_REQUEST* requestInitWithParams(int requestType, const char* url, HEADER* headers, const char* content, int portNumber);
void requestFree(HTTP_REQUEST** request);

HTTP_RESPONSE* responseInit();
HTTP_RESPONSE* responseInitWithParams(const char* responseStatus, HEADER* headers, const char* content);
void responseFree(HTTP_RESPONSE** response);

static char** parseNameValueString(const char* nameValue);
HEADER* initHeaders(int count, ...);
static void addToHeaders(HEADER** headers, const char* nameValue);
HEADER* findHeader(HEADER* headers, const char* name);
static void headersFree(HEADER** headers);

void setHttpRequestType(HTTP_REQUEST** request, int type);
void setHttpRequestUrl(HTTP_REQUEST** request, const char* url);
void addHttpRequestHeader(HTTP_REQUEST** request, HEADER* httpHeader);
void setHttpRequestData(HTTP_REQUEST** request, const char* data);

char* convertHttpRequestToString(HTTP_REQUEST* request);
HTTP_REQUEST* convertHttpRequestToObject(const char* request);

static bool checkHttpRequest(HTTP_REQUEST* request);
HTTP_RESPONSE* sendHttpRequest(HTTP_REQUEST** request);

void setHttpResponseStatus(HTTP_RESPONSE** response, const char* status);
void addHttpResponseHeader(HTTP_RESPONSE** response, HEADER* httpHeader);
void setHttpResponseData(HTTP_RESPONSE** response, const char* data);

char* convertHttpResponseToString(HTTP_RESPONSE* response);
HTTP_RESPONSE* convertHttpResponseToObject(const char* response);

static bool checkHttpResponse(HTTP_RESPONSE* response);
HTTP_REQUEST* getHttpRequest(int sfd);
void sendHttpResponse(int sfd, HTTP_RESPONSE** response);

// set method GET POST PUT DELETE + data
// set headers Content-Type: application/json and ect
//             application/json by default - don't plan to send other types
// set URL - when add also add header HOST first part from https:// to /, then address from / to end

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

// define HTTP statuses

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