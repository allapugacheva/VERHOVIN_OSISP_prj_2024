#pragma once
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "checkingFunctions.h"
#include <time.h>

// Библиотека для чтения данных с датчиков одноплатного компьютера.

#define DHTPIN 4                          // Номер GPIO для датчика температуры и влажности.
#define MICROPIN 17                       // Номер GPIO для микрофона.

typedef struct raspberry_sensors {        // Данные с одноплатного компьютера.

    char* dateTime;                       // Текущая дата и время.
    double temperature;                   // Температура.
    double humidity;                      // Влажность.
    bool hasLoud;                         // Наличие звука.
} RASPBERRY_SENSORS;

RASPBERRY_SENSORS* readData();            // Функция чтения данных с датчиков.

// ---------------------------------------------------------------------------------------------------------------------

static uint8_t parsePulse(int level);                                   // Чтение бита данных с датчика температуры и влажности.
static bool readDht11Data(double* temperature, double* humidity);      // Чтение данных с датчика температуры и влажности.