#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include "json.h"
#include "http.h"
#include "queue.h"
#include "fileFunctions.h"
#include "raspberryFunctions.h"

typedef struct raspberry_data {
    char* processor;
    double minFrequency;
    double maxFrequency;
    double curFrequency;
    double curTemperature;
    double totalMemory;
    double availableMemory;
    double workTime;
} RASPBERRY_DATA;