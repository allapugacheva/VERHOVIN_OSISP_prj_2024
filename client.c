#include "client.h"

bool continuing = true;
QNODE* queue = NULL;
pthread_mutex_t mutexQueue;
pthread_cond_t condQueue;

// Запуск: ./client 192.168.100.62 data.txt
int main(int argc, char* argv[]) {

    sPush(&checkStack1, "main");
    sPush(&checkStack2, "main");

    freopen("clientErrors.log", "w", stderr);

    if (argc < 3) {
        printf("Server address not specified.\n");
        exit(0);
    }

    if(wiringPiSetupGpio() == -1) {                                                                                // Инициализация библиотеки для работы с GPIO одноплатного компьютера.
        handleError("error while initialise GPIO", false, NULL, 0, NULL, 0, NULL);
        printf("Client stopped work with error. To see more information open file 'clientErrors.log'.\n");
        sPop(&checkStack1);
        sPop(&checkStack2);
        exit(0);
    }

    initNums();

    signal(SIGUSR1, stop);                                                                            // Сигнал завершения работы.
    signal(SIGUSR2, err);

    pthread_cond_init(&condQueue, NULL);                                                           // Инициализация условной переменной и мьютекса.
    pthread_mutex_init(&mutexQueue, NULL);


    if(pthread_create(&thread1, NULL, readHandler, argv[2]) != 0) {                 // Поток чтения данных с одноплатного компьютера.
        handleError("error while open reader.", false, NULL, 0, NULL, 0, NULL);
        pthread_kill(thread1, SIGUSR2);
        sleep(1);
    }

    if(continuing != error)
        if(pthread_create(&thread2, NULL, sendHandler, argv[1]) != 0) {             // Поток пересылки данных на сервер.
            handleError("error while open sender.", false, NULL, 0, NULL, 0, NULL);
            pthread_kill(thread1, SIGUSR2);
            sleep(1);
        }

    if(continuing != error) {
        printf("Press any key to stop...\n");
        getchar();
        pthread_kill(thread1, SIGUSR1);                                                            // Сигнал завершения работы.
    } else
        printf("Client stopped work with error. To see more information open file 'clientErrors.log'.\n");

    pthread_join(thread1, NULL);                                                                  // Ожидание завершения работы потоков.
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutexQueue);
    pthread_cond_destroy(&condQueue);

    sPop(&checkStack1);
    sPop(&checkStack2);

    freeNums();

    return 0;
}