#include "http.h"

int initClient(const char* address, int portNumber) {

    struct hostent* server;
    struct sockaddr_in addr;
    if(portNumber == -1)
        portNumber = 8080;

    server = gethostbyname(address);

    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber); // ????
    bcopy((char*)server->h_addr_list[0], (char*)&addr.sin_addr.s_addr, server->h_length);

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        close(sfd);
        return -1;
    }

    while(connect(sfd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {

        if(errno == ENOENT) {
            sleep(1);
            continue;
        } else {
            close(sfd);
            return -1;
        }
    }

    return sfd;
}
int initServer(int portNumber) {

    if(portNumber == -1)
        portNumber = 8080;
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(portNumber);
    addr.sin_addr.s_addr = INADDR_ANY;

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sfd == -1) {
        close(sfd);
        return -1;
    }

    int opt = 1;
    if(setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(sfd);
        return -1;
    }

    if(bind(sfd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        close(sfd);
        return -1;
    }

    if(listen(sfd, 50) == -1) {
        close(sfd);
        return -1;
    }

    return sfd;
}

HTTP_REQUEST* requestInit() {

    HTTP_REQUEST* request = REQUEST_MALLOC;

    request->content = NULL;
    request->headers = NULL;
    request->url = NULL;
    request->port = -1;

    return request;
}
HTTP_REQUEST* requestInitWithParams(int requestType, const char* url, HEADER* headers, const char* content, int portNumber) {

    checkPointer(url, "requestInitWithParams", "url");

    HTTP_REQUEST* request = REQUEST_MALLOC;

    request->requestType = requestType;
    copyString(&request->url, url);
    if(content != NULL)
        copyString(&request->content, content);
    else
        request->content = NULL;
    request->headers = headers;
    request->port = portNumber;

    return request;
}
void requestFree(HTTP_REQUEST** request) {

    headersFree(&(*request)->headers);
    free((*request)->content);
    free((*request)->url);
    free(*request);

    *request = NULL;
}

HTTP_RESPONSE* responseInit() {

    HTTP_RESPONSE* response = RESPONSE_MALLOC;

    response->headers = NULL;
    response->content = NULL;
    response->responseStatus = NULL;

    return response;
}
HTTP_RESPONSE* responseInitWithParams(const char* responseStatus, HEADER* headers, const char* content) {

    checkPointer(responseStatus, "responseInitWithParams", "responseStatus");

    HTTP_RESPONSE* response = RESPONSE_MALLOC;

    response->headers = headers;
    if(content != NULL)
        copyString(&response->content, content);
    else
        response->content = NULL;
    copyString(&response->responseStatus, responseStatus);

    return response;
}
void responseFree(HTTP_RESPONSE** response) {

    headersFree(&(*response)->headers);
    free((*response)->content);
    free((*response)->responseStatus);

    *response = NULL;
}

static char** parseNameValueString(const char* nameValue) {

    char **result = (char **) malloc(2 * sizeof(char *));
    int i = 0;

    result[0] = copyStringFromIToSymbol(nameValue, &i, ':');
    i += 2;
    result[1] = copyStringFromIToSymbol(nameValue, &i, '\0');

    return result;
}
HEADER* initHeaders(int count, ...) {

    checkNumber(count, 1, -1, "initHeaders");

    va_list args;
    va_start(args, count);

    HEADER* headers = NULL;
    for(int i = 0; i<count; i++) {
        const char* temp = va_arg(args, const char*);
        addToHeaders(&headers, temp);
    }
    va_end(args);

    return headers;
}
static void addToHeaders(HEADER** headers, const char* nameValue) {

    HEADER* tempHeader = *headers;

    if(tempHeader == NULL) {
        *headers = HEADER_MALLOC;
        (*headers)->next = NULL;
        (*headers)->prev = NULL;
        tempHeader = *headers;
    } else {
        while (tempHeader->next != NULL)
            tempHeader = tempHeader->next;

        tempHeader->next = HEADER_MALLOC;
        tempHeader->next->next = NULL;
        tempHeader->next->prev = tempHeader;
        tempHeader = tempHeader->next;
    }

    char** result = parseNameValueString(nameValue);
    copyString(&tempHeader->name, result[0]);
    copyString(&tempHeader->value, result[1]);

    free(result[0]);
    free(result[1]);
    free(result);
}
HEADER* findHeader(HEADER* headers, const char* name) {

    HEADER* result = headers;

    while(result != NULL && compareString(result->name, name) != 0)
        result = result->next;

    return result;
}
static void headersFree(HEADER** headers) {

    while(*headers != NULL) {

        free((*headers)->value);
        free((*headers)->name);

        HEADER* temp = *headers;
        *headers = (*headers)->next;
        free(temp);
    }
}

void setHttpRequestType(HTTP_REQUEST** request, int type) {

    checkDoublePointer((void**)request, "setHttpRequestType", "request");
    checkNumber(type, 0, 3, "setHttpRequestType");

    (*request)->requestType = type;
}
void setHttpRequestUrl(HTTP_REQUEST** request, const char* url) {

    checkDoublePointer((void**)request, "setHttpRequestUrl", "request");
    checkPointer(url, "setHttpRequestUrl", "url");

    char* host, *address;
    int i = 0, iLen = stringLength(url);
    for(;i < iLen && url[i] != '/' && url[i] != ':'; i++);

    if(i < iLen && url[i+1] == '/')
        i+=2;
    else
        i = 0;

    bool hasPort = false;
    for(int j = i; url[j] != '/'; j++)
        if(url[j] == ':') {
            hasPort = true;
            break;
        }

    if(hasPort) {

        host = copyStringFromIToSymbol(url, &i, ':');
        i++;
        (*request)->port = (int)CTOD(copyStringFromIToSymbol(url, &i, '/'));
    }
    else
        host = copyStringFromIToSymbol(url, &i, '/');
    if(i < iLen)
        address = copyStringFromIToSymbol(url, &i, '\0');
    else
        address = "/";

    char* header = (char*)malloc(7 + stringLength(host));
    sprintf(header, "Host: %s", host);
    addHttpRequestHeader(request, initHeaders(1, header));
    free(header);
    copyString(&(*request)->url, address);
}
void addHttpRequestHeader(HTTP_REQUEST** request, HEADER* httpHeader) {

    checkDoublePointer((void**)request, "addHttpRequestHeader", "request");
    checkPointer(httpHeader, "addHttpRequestHeader", "httpHeader");

    if((*request)->headers == NULL)
        (*request)->headers = httpHeader;
    else {
        HEADER* tempHeader = (*request)->headers;

        while(tempHeader->next != NULL)
            tempHeader = tempHeader->next;

        tempHeader->next = httpHeader;
        httpHeader->prev = tempHeader;
    }
}
void setHttpRequestData(HTTP_REQUEST** request, const char* data) {

    checkDoublePointer((void**)request, "setHttpRequestData", "request");
    checkPointer(data, "setHttpRequestData", "data");

    copyString(&(*request)->content, data);

    char* header = (char*)malloc(17 + findNumberLen(stringLength(data)));
    sprintf(header, "Content-Length: %d", stringLength(data));
    addHttpRequestHeader(request, initHeaders(1, header));
}

static int findHttpRequestStringLength(HTTP_REQUEST* request) {

    int length = 15; // space between request type and url, url and version, \n after version, \n after headers, http version
    switch(request->requestType) {
        case put_request:
        case get_request:
            length+= 3;
            break;
        case post_request:
            length += 4;
            break;
        case delete_request:
            length += 6;
            break;
    }
    length += stringLength(request->url);
    if(request->content != NULL)
        length += stringLength(request->content) + 1;

    for(HEADER* header = request->headers; header != NULL; header = header->next) {
        length += stringLength(header->name);
        length += 4;
        length += stringLength(header->value);
    }

    return length;
}

char* convertHttpRequestToString(HTTP_REQUEST* request) {

    int stringLength = findHttpRequestStringLength(request);
    char *result = (char *) malloc(1);
    result[0] = '\0';

    char *temp = (char *) malloc(256);
    switch (request->requestType) {
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

    for (HEADER *header = request->headers; header != NULL; header = header->next) {
        temp = (char *) malloc(256);

        sprintf(temp, "%s: %s\r\n", header->name, header->value);
        addString(&result, temp);

        free(temp);
    }
    addString(&result, "\r\n");

    if (request->content != NULL)
        addString(&result, request->content);

    result[stringLength - 1] = '\0';

    return result;
}
HTTP_REQUEST* convertHttpRequestToObject(const char* request) {

    HTTP_REQUEST* result = REQUEST_MALLOC;

    int i = 0;
    switch (request[0]) {
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
    copyString(&result->url, copyStringFromIToSymbol(request, &i, ' '));

    while(request[i++] != '\n');

    HEADER* headers = NULL;
    while(i < stringLength(request) && request[i] != '\r' && request[i] != '\0') {

        addToHeaders(&headers, copyStringFromIToSymbol(request, &i, '\r'));
        if(request[i] != '\0')
            i+=2;
    }
    result->headers = headers;

    i+=2;
    if(i < stringLength(request))
        copyString(&result->content, copyStringFromIToSymbol(request, &i, '\0'));
    else
        result->content = NULL;

    return result;
}

static bool checkHttpRequest(HTTP_REQUEST* request) {

    if(request->url == NULL)
        return false;
    if(request->headers == NULL)
        return false;

    if(findHeader(request->headers, "Host") == NULL || findHeader(request->headers, "User-Agent") == NULL)
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
HTTP_RESPONSE* sendHttpRequest(HTTP_REQUEST** request) {

    checkDoublePointer((void**)request, "sendHttpRequest", "request");

    if ((*request)->content == NULL && (*request)->requestType != get_request)
        addHttpRequestHeader(request, initHeaders(1, "Content-Length: 0"));

    if (!checkHttpRequest(*request)) {
        printf("Not enough data in request.\n");
        exit(0);
    }
    int sfd = initClient(findHeader((*request)->headers, "Host")->value, (*request)->port);
    if(sfd == -1)
        return NULL;

    char *httpRequestString = convertHttpRequestToString(*request), *httpResponseString = (char *) malloc(512);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sfd, &readfds);

    if (send(sfd, httpRequestString, stringLength(httpRequestString), 0) <= 0) {
        printf("Error while send request.\n");
        close(sfd);
        exit(0);
    }

    int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);
    if(ready <= 0) {
        printf("Error while reading request.\n");
        close(sfd);
        exit(0);
    }

    if (recv(sfd, httpResponseString, 512, 0) <= 0) {
        printf("Error while receive response.\n");
        close(sfd);
        exit(0);
    }

    HTTP_RESPONSE *response = convertHttpResponseToObject(httpResponseString);

    if (compareString(response->responseStatus, HTTP_STATUS_200) != 0) {
        printf("Error while send request.\n");
        close(sfd);
        exit(0);
    }

    free(httpRequestString);
    free(httpResponseString);
    close(sfd);

    return NULL;
}

void setHttpResponseStatus(HTTP_RESPONSE** response, const char* status) {

    checkDoublePointer((void**)response, "setHttpResponseStatus", "response");
    checkPointer(status, "setHttpResponseStatus", "status");

    copyString(&(*response)->responseStatus, status);
}
void addHttpResponseHeader(HTTP_RESPONSE** response, HEADER* httpHeader) {

    checkDoublePointer((void**)response, "addHttpResponseHeader", "response");
    checkPointer(httpHeader, "addHttpResponseHeader", "httpHeader");

    if((*response)->headers == NULL)
        (*response)->headers = httpHeader;
    else {
        HEADER* tempHeader = (*response)->headers;

        while(tempHeader->next != NULL)
            tempHeader = tempHeader->next;

        tempHeader->next = httpHeader;
        httpHeader->prev = tempHeader;
    }
}
void setHttpResponseData(HTTP_RESPONSE** response, const char* data) {

    checkDoublePointer((void**)response, "setHttpResponseData", "response");
    checkPointer(data, "setHttpResponseData", "data");

    copyString(&(*response)->content, data);

    char* header = (char*)malloc(17 + findNumberLen(stringLength(data)));
    sprintf(header, "Content-Length: %d", stringLength(data));
    addHttpResponseHeader(response, initHeaders(1, header));
}

static int findHttpResponseLength(HTTP_RESPONSE* response) {

    int length = 14; // space between version and status, \n after status, \n after headers, http version

    if(response->content != NULL)
        length += stringLength(response->content);
    length += stringLength(response->responseStatus);

    for(HEADER* header = response->headers; header != NULL; header = header->next) {
        length += stringLength(header->name);
        length += 4;
        length += stringLength(header->value);
    }

    return length;
}

char* convertHttpResponseToString(HTTP_RESPONSE* response) {

    int stringLength = findHttpResponseLength(response);
    char *result = (char *) malloc(stringLength);
    result[0] = '\0';

    char *temp = (char *) malloc(256);
    sprintf(temp, "%s %s\r\n", HTTP_PROTOCOL_VERSION, response->responseStatus);
    addString(&result, temp);
    free(temp);

    for (HEADER *header = response->headers; header != NULL; header = header->next) {
        temp = (char *) malloc(256);

        sprintf(temp, "%s: %s\r\n", header->name, header->value);
        addString(&result, temp);

        free(temp);
    }
    addString(&result, "\r\n");

    if (response->content != NULL)
        addString(&result, response->content);

    result[stringLength - 1] = '\0';

    return result;
}
HTTP_RESPONSE* convertHttpResponseToObject(const char* response) {

    HTTP_RESPONSE * result = RESPONSE_MALLOC;
    int i = 0;

    while(response[i++] != ' ');

    copyString(&result->responseStatus, copyStringFromIToSymbol(response, &i, '\r'));
    i+=2;

    HEADER* headers = NULL;
    while(i < stringLength(response) && response[i] != '\r') {

        addToHeaders(&headers, copyStringFromIToSymbol(response, &i, '\r'));
        i+=2;
    }
    result->headers = headers;

    i+=2;
    if(i < stringLength(response))
        copyString(&result->content, copyStringFromIToSymbol(response, &i, '\0'));
    else
        result->content = NULL;

    return result;
}

static bool checkHttpResponse(HTTP_RESPONSE* response) {

    if(response->responseStatus == NULL)
        return false;
    if(response->headers == NULL)
        return false;
    if(findHeader(response->headers, "Content-Length") == NULL)
        return false;
    if(response->content != NULL)
        if(findHeader(response->headers, "Content-Type") == NULL)
            return false;

    return true;
}
HTTP_REQUEST* getHttpRequest(int sfd) {

    char *httpRequestString = (char *) malloc(512);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(sfd, &readfds);

    int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);
    if(ready <= 0) {
        printf("Error while receiving request.\n");
        exit(0);
    }

    if (recv(sfd, httpRequestString, 512, 0) <= 0) {
        HTTP_RESPONSE* response = responseInit();
        copyString(&response->responseStatus, HTTP_STATUS_500);
        addHttpResponseHeader(&response, initHeaders(1, "Content-Length: 0"));
        char* httpResponseString = convertHttpResponseToString(response);

        if (send(sfd, httpResponseString, stringLength(httpResponseString), 0) <= 0) {
            printf("Error while send response.\n");
            free(httpResponseString);
            responseFree(&response);
            exit(0);
        }
        free(httpResponseString);
        responseFree(&response);
    }

    return convertHttpRequestToObject(httpRequestString);
}
void sendHttpResponse(int sfd, HTTP_RESPONSE** response) {

    checkDoublePointer((void**)response, "sendHttpResponse", "response");

    if ((*response)->content == NULL)
        addHttpResponseHeader(response, initHeaders(1, "Content-Length: 0"));

    if (!checkHttpResponse(*response)) {
        printf("Not enough data in response.\n");
        exit(0);
    }

    char *httpResponseString = convertHttpResponseToString(*response);

    if (send(sfd, httpResponseString, stringLength(httpResponseString), 0) <= 0) {
        printf("Error while send response.\n");
        exit(0);
    }
}