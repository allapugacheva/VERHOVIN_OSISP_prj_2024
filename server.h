#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "http.h"
#include "queue.h"
#include "fileFunctions.h"

// Сервер для обработки данных.

extern bool continuing;                 // Флаг для остановки потоков программы.
extern QNODE* queue;                     // Очередь сообщений на запись.
extern pthread_cond_t condQueue;        // Условная переменная для работы с очередью сообщений.
extern pthread_mutex_t mutexQueue;      // Мьютекс для работы с очередью сообщений.

void *fillerHandler(void* arg);         // Обработчик записи сообщений в файл.
void *receiveHandler(void *arg);        // Обработчик приёма сообщений от клиента.
void stop();                            // Обработчик сигнала завершения работы.
void err();                             // Обработчик сигнала ошибки.