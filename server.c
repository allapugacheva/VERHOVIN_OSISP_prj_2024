#include "header.h"

bool continuing = true;
NODE* queue = NULL;
pthread_cond_t condQueue;
pthread_mutex_t mutexQueue;

void* fillerHandler(void* arg) {

    char* fileName = (char*)arg;

    while(continuing) {

        pthread_mutex_lock(&mutexQueue);

        while (queue == NULL) {
            if(!continuing) {
                pthread_mutex_unlock(&mutexQueue);
                return NULL;
            }
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        addStringToFile(fileName, queue->data);
        pop(&queue);

        pthread_mutex_unlock(&mutexQueue);
    }
}

void *receiveHandler(void *arg) {

    int sfd = initServer(*(int*)arg);

    struct timeval timeout;
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    fd_set readfds;

    while(continuing) {

        FD_ZERO(&readfds);
        FD_SET(sfd, &readfds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int ready = select(sfd + 1, &readfds, NULL, NULL, &timeout);
        if(ready <= 0) {
            continue;
        }

        if (FD_ISSET(sfd, &readfds)) {

            int cfd = accept(sfd, NULL, 0);
            if (cfd == -1) {
                printf("Error while client connection.\n");
                close(sfd);
                close(cfd);
                exit(0);
            }

            HTTP_REQUEST *request = getHttpRequest(cfd);

            pthread_mutex_lock(&mutexQueue);

            push(&queue, request->content);
            if (queue->next == NULL)
                pthread_cond_signal(&condQueue);

            pthread_mutex_unlock(&mutexQueue);

            requestFree(&request);

            HTTP_RESPONSE *response = responseInitWithParams(HTTP_STATUS_200, NULL, NULL);
            sendHttpResponse(cfd, &response);
            responseFree(&response);
        }
    }

    pthread_cond_signal(&condQueue);
}

void stop() {
    continuing = false;
}

int main(int argc, char* argv[]) {

    if(argc < 2) {
        printf("Not enough arguments.\n");
        exit(0);
    }
    int portNumber = argc == 3 ? (int)CTOD(argv[3]) : -1;

    signal(SIGUSR1, stop);

    pthread_cond_init(&condQueue, NULL);
    pthread_mutex_init(&mutexQueue, NULL);

    pthread_t handlerThread;
    if(pthread_create(&handlerThread, NULL, receiveHandler, &portNumber) != 0) {
        printf("Error while open server.\n");
        exit(0);
    }
    pthread_t fillerTread;
    if(pthread_create(&fillerTread, NULL, fillerHandler, argv[1]) != 0) {
        printf("Error while open filler.\n");
        exit(0);
    }

    while(continuing) {

        printf("1) Show data\n2) Sort data\n3) Clear data\n4) Exit\nSelect variant: ");
        switch (getchar()) {
            case '1':

                showContent(argv[1]);
                break;
            case '2': {
                while (getchar() != '\n')
                    continue;

                printf("1) DateAndTime\n2) Temperature\n3) Humidity\n4) Loud\nSelect field to sort by: ");
                switch (getchar()) {
                    case '1':
                        sortContent(argv[1], "dateAndTime");
                        break;
                    case '2':
                        sortContent(argv[1], "temperature");
                        break;
                    case '3':
                        sortContent(argv[1], "humidity");
                        break;
                    case '4':
                        sortContent(argv[1], "hasLoud");
                        break;
                }
                break;
            }
            case '3':
                clearFile(argv[1]);
                break;
            case '4':
                pthread_kill(handlerThread, SIGUSR1);
                pthread_join(handlerThread, NULL);
                pthread_join(fillerTread, NULL);
                break;
        }
        while (getchar() != '\n')
            continue;
    }

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    return 0;
}