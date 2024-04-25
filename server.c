#include "server.h"

bool continuing = true;                                                                            // Флаг для остановок потоков программы.
QNODE* queue = NULL;                                                                               // Очередь сообщений на запись.
pthread_cond_t condQueue;                                                                          // Условная переменная для работы с очередью сообщений.
pthread_mutex_t mutexQueue;                                                                        // Мьютекс для работы с очередью сообщений.

// Запуск: ./server filename portNumber(8080 если не указан)
int main(int argc, char* argv[]) {

    sPush(&checkStack1, "main");
    sPush(&checkStack2, "main");

    freopen("serverErrors.log", "w", stderr);

    initNums();

    if(argc < 2) {
        handleError("not enough arguments.", false, NULL, 0, NULL, 0, NULL);
        printf("Server stopped work with error. To see more information open file 'serverErrors.log'.\n");
        sPop(&checkStack1);
        sPop(&checkStack2);
        exit(0);
    }
    int portNumber = argc == 3 ? (int)stringToDouble(argv[3]) : -1;                            // Обработка порта сервера.

    signal(SIGUSR1, stop);                                                             // Установка сигнала завершения работы.
    signal(SIGUSR2, err);

    pthread_cond_init(&condQueue, NULL);                                            // Инициализация условной переменной и мьютекса.
    pthread_mutex_init(&mutexQueue, NULL);

    if(pthread_create(&thread1, NULL, receiveHandler, &portNumber) != 0) {
        handleError("error while start reader thread.", false, &continuing, 0, NULL, 0, NULL);
        pthread_kill(thread1, SIGUSR2);
        sleep(1);
    }

    if(continuing != error)
        if(pthread_create(&thread2, NULL, fillerHandler, argv[1]) != 0) {
            handleError("error while start filler thread.", false, &continuing, 0, NULL, 0, NULL);
            pthread_kill(thread1, SIGUSR2);
            sleep(1);
        }

    while(continuing == true) {                                                                   // Базовой меню для работы с данными.

        printf("1) Show data\n2) Sort data\n3) Clear data\n4) Exit\nSelect variant: ");
        int ch = getchar();
        system("clear");
        switch (ch) {
            case '1':
                if(showContent(argv[1]) == error) {                                      // Вывод данных на экран.
                    handleError("error while show content.", false, NULL, 0, NULL, 0, NULL);
                    pthread_kill(thread1, SIGUSR2);
                    sleep(1);
                }
                break;
            case '2': {
                while (getchar() != '\n')                                                         // Сортировка данных по полю.
                    continue;

                printf("1) DateAndTime\n2) Temperature\n3) Humidity\n4) Loud\nSelect field to sort by: ");
                char* tempField = NULL;
                switch (getchar()) {
                    case '1':
                        copyString(&tempField, "dateAndTime");
                        break;
                    case '2':
                        copyString(&tempField, "temperature");
                        break;
                    case '3':
                        copyString(&tempField, "humidity");
                        break;
                    case '4':
                        copyString(&tempField, "hasLoud");
                        break;
                }

                if(sortContent(argv[1], tempField) == error) {
                    handleError("error while sort by field.", false, NULL, 0, NULL, 0, NULL);
                    pthread_kill(thread1, SIGUSR2);
                    sleep(1);
                }

                free(tempField);
                break;
            }
            case '3':
                if(clearFile(argv[1]) == error) {                                       // Очистить файл.
                    handleError("error while clear file.", false, NULL, 0, NULL, 0, NULL);
                    pthread_kill(thread1, SIGUSR2);
                    sleep(1);
                }
                break;
            default:
                pthread_kill(thread1, SIGUSR1);                                    // Завершение работы.
                sleep(1);
                break;
        }
        while (getchar() != '\n')
            continue;
    }

    pthread_join(thread1, NULL);                                                 // Ожидание завершения процессов.
    pthread_join(thread2, NULL);

    pthread_mutex_destroy(&mutexQueue);                                                  // Деинициализация мьютекса и условной переменной.
    pthread_cond_destroy(&condQueue);

    while(queue != NULL) {
        if(addStringToFile(argv[1], queue->data) == error) {
            handleError("error while add data from queue to file.", false, &continuing, 0, NULL, 0, NULL);

            while(queue != NULL)
                qPop(&queue);

            break;
        }
        qPop(&queue);
    }

    if(continuing == error)
        printf("Server stopped work with error. To see more information open file 'serverErrors.log'.\n");
    else
        printf("Server successfully finished work.\n");

    sPop(&checkStack1);
    sPop(&checkStack2);

    freeNums();

    return 0;
}