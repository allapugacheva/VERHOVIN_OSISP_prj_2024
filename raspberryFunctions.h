#pragma once
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "stringFunctions.h"
#include <time.h>

#define DHTPIN 7
#define RELAYPIN 13
#define MICROPIN 11
int dht11_dat[5] = {0, 0, 0, 0, 0};
//int olddht11_dat[5] = {0, 0, 0, 0, 0};

int maxcycles = 1000;

typedef struct raspberry_sensors {

    char* dateTime;
    double temperature;
    double humidity;
    bool hasLoud;
} RASPBERRY_SENSORS;

RASPBERRY_SENSORS* readData();