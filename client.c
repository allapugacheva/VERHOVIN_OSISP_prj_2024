#include "header.h"

bool continuing = true;
NODE* queue = NULL;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

void *sendHandler(void* arg) {

    while(continuing) {

        pthread_mutex_lock(&mutexQueue);

        while(queue == NULL) {
            if(!continuing) {
                pthread_mutex_unlock(&mutexQueue);
                return NULL;
            }
            pthread_cond_wait(&condQueue, &mutexQueue);
        }

        HTTP_REQUEST *request = requestInit();

        setHttpRequestUrl(&request, (char*)arg);
        setHttpRequestType(&request, post_request);
        addHttpRequestHeader(&request, initHeaders(2, "User-Agent: Raspberry pi zero w", "Content-Type: application/json"));

        setHttpRequestData(&request, queue->data);

        pthread_mutex_unlock(&mutexQueue);

        if(sendHttpRequest(&request) != NULL)
            pop(&queue);
        else
            sleep(5);

        requestFree(&request);
    }
}

void *readHandler(void* arg) {

    readToQueue(&queue, (char*)arg);
    pthread_cond_signal(&condQueue);

    RASPBERRY_DATA data;

    while(continuing) {

//        FILE *f = openFile("/proc/cpuinfo", "r");
//
//        char *temp = (char *) malloc(80);
//
//        // processor
//        fgets(temp, 80, f);
//        fgets(temp, 80, f);
//        temp[stringLength(temp) - 1] = '\0';
//        int i = 0;
//        while (temp[i++] != ':');
//        i++;
//        copyString(&data.processor, copyStringFromIToSymbol(temp, &i, '\0'));
//
//        fclose(f);
//
//        // min frequency
//        f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq", "r");
//
//        fgets(temp, 80, f);
//        data.minFrequency = CTOD(temp);
//
//        fclose(f);
//
//        // max frequency
//        f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq", "r");
//
//        fgets(temp, 80, f);
//        data.maxFrequency = CTOD(temp);
//
//        fclose(f);
//
//        // cur frequency
//        f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq", "r");
//
//        fgets(temp, 80, f);
//        data.curFrequency = CTOD(temp);
//
//        fclose(f);
//
//        // cur temp
//        f = openFile("/sys/class/thermal/thermal_zone0/temp", "r");
//
//        fgets(temp, 80, f);
//        data.curTemperature = CTOD(temp) / 1000;
//
//        fclose(f);
//
//        // total memory
//        f = openFile("/proc/meminfo", "r");
//
//        fgets(temp, 80, f);
//        data.totalMemory = CTOD(temp);
//
//        // available memory
//        fgets(temp, 80, f);
//        fgets(temp, 80, f);
//        data.availableMemory = CTOD(temp);
//
//        fclose(f);
//
//        // work time
//        f = openFile("/proc/uptime", "r");
//
//        fgets(temp, 80, f);
//        data.workTime = CTOD(temp);
//
//        fclose(f);
//        free(temp);

        RASPBERRY_SENSORS* tempData = readData();

        pthread_mutex_lock(&mutexQueue);

                push(&queue, parseJsonToString(createObject(4, (OBJECT_FIELD[]){
                {"dateAndTime", string, {.stringVal = tempData->dateTime}, stringLength(tempData->dateTime)},
                {"temperature", number, {.numberVal = tempData->temperature}, 0},
                {"humidity", number, {.numberVal = tempData->humidity}, 0},
                {"hasLoud", number, {.boolVal = tempData->hasLoud}, 0}
        })));

//        push(&queue, parseJsonToString(createObject(8, (OBJECT_FIELD[]){
//                {"processor", string, {.stringVal = data.processor}, stringLength(data.processor)},
//                {"minFrequency", number, {.numberVal = data.minFrequency}, 0},
//                {"maxFrequency", number, {.numberVal = data.maxFrequency}, 0},
//                {"curFrequency", number, {.numberVal = data.curFrequency}, 0},
//                {"curTemperature", number, {.numberVal = data.curTemperature}, 0},
//                {"totalMemory", number, {.numberVal = data.totalMemory}, 0},
//                {"availableMemory", number, {.numberVal = data.availableMemory}, 0},
//                {"workTime", number, {.numberVal = data.workTime}, 0}
//        })));
        if(queue->next == NULL)
            pthread_cond_signal(&condQueue);

        pthread_mutex_unlock(&mutexQueue);

        sleep(10);
    }

    if(queue != NULL)
        writeFromQueue(queue, (char*)arg);
    else
        clearFile((char*)arg);

    pthread_cond_signal(&condQueue);
}

void stop() {
    continuing = false;
}

int main(int argc, char* argv[]) {

    if (argc < 3) {
        printf("Server address not specified.\n");
        exit(0);
    }

    signal(SIGUSR1, stop);

    pthread_cond_init(&condQueue, NULL);
    pthread_mutex_init(&mutexQueue, NULL);

    pthread_t readThread;
    if(pthread_create(&readThread, NULL, readHandler, argv[2]) != 0) {
        printf("Error while open reader.\n");
        exit(0);
    }
    pthread_t sendTread;
    if(pthread_create(&sendTread, NULL, sendHandler, argv[1]) != 0) {
        printf("Error while open sender.\n");
        exit(0);
    }

    printf("Press any key to stop...\n");
    getchar();

    pthread_kill(readThread, SIGUSR1);
    pthread_join(readThread, NULL);
    pthread_join(sendTread, NULL);

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    return 0;

}