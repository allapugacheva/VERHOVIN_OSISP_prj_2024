#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "http.h"
#include "fileFunctions.h"
#include "raspberryFunctions.h"

extern bool continuing;                                // Переменная для завершения работы.
extern QNODE* queue;                                    // Очередь сообщений.
extern pthread_mutex_t mutexQueue;                     // Мьютекс для работы с очередью в нескольких потоках.
extern pthread_cond_t condQueue;                       // Условная переменная для ожидания заполнения очереди.

void *sendHandler(void* arg);                          // Обработчик отправки сообщений на сервер.
void *readHandler(void* arg);                          // Обработчик считывания данных с одноплатного компьютера.
void stop();                                           // Обработчик сигнала завершения работы.
void err();                                            // Обработчик сигнала ошибочного завершения работы.