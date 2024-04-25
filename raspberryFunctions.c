#include "raspberryFunctions.h"

int dht11_dat[5] = {0, 0, 0, 0, 0};                          // Данные с датчика температуры и влажности.
int maxCycles = 255;                                                             // Максимальное количество попыток считать бит при передаче данных с датчика температуры и влажности.

// Чтение бита данных с датчика температуры и влажности.
static uint8_t parsePulse(int level) {

    int count = 0;
    while(digitalRead(DHTPIN) == level) {                                        // Чтение бита и проверка, что он равен указанному уровню сигнала.
        delayMicroseconds(1);                                                    // Ожидание одну микросекунду.
        if (count++ >= maxCycles)
            return -1;
    }

    return count;
}

// Чтение данных с датчика температуры и влажности.
static bool readDht11Data(double* temperature, double* humidity) {

    pthread_self() == thread1 ? sPush(&checkStack1, "readDht11Data") : sPush(&checkStack2, "readDht11Data");

    for(int i = 0; i<5; i++)                                                     // Очистка предыдущих данных.
        dht11_dat[i] = 0;

    pinMode(DHTPIN, OUTPUT);                                                     // Установка режима записи на GPIO.
    digitalWrite(DHTPIN, LOW);                                                   // Подачи низкого уровня сигнала для сообщения датчику, что компьютер хочет считать данные.
    delay(18);                                                                   // Ожидание 18 миллисекунд, чтобы датчик принял сигнал.
    digitalWrite(DHTPIN, HIGH);                                                  // Подача сигнала датчику, что компьютер готов к приёму сигнала.
    delayMicroseconds(40);                                                       // Ожидание ответа от датчика.
    pinMode(DHTPIN, INPUT);                                                      // Установка режима чтения с GPIO.

    if(parsePulse(LOW) == -1)
        return *(bool*)handleError("error while receive LOW level.", true, NULL, 1, NULL, 0, NULL);
    if(parsePulse(HIGH) == -1)
        return *(bool*)handleError("error while receive HIGH level.", true, NULL, 1, NULL, 0, NULL);

    uint8_t cycles[80];                                                          // Данные с датчика.
    for (int i = 0; i < 80; i+=2 ) {                                             // Ожидание 40 бит.

        cycles[i] = parsePulse(LOW);                                       // Низкий уровень сигнала информирует о начале передачи бита.
        cycles[i+1] = parsePulse(HIGH);                                    // По длительности высокого уровня сигналов получаем бит (26-28 мкс - "0", 70 мкс - "1").
    }

    for(int i = 0; i<40; i++) {                                                 // Обработка 40 полученных бит.

        if(cycles[2*i] == -1 || cycles[2*i+1] == -1)
            return *(bool*)handleError("error while read data from dht11.", true, NULL, 1, NULL, 0, NULL);

        dht11_dat[i/8] <<= 1;                                                   // Сдвиг результата.

        if(cycles[2*i] < cycles[2*i + 1])                                       // Проверка, что пришёл сигнал логической "1".
            dht11_dat[i/8] |= 1;                                                // Установка бита в 1.
    }
                                                                                // Сверка контрольной суммы.
    if (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) {
        *humidity = dht11_dat[0] + (double)dht11_dat[1] / divisorForDouble(dht11_dat[1]);     // Получение значений влажности и температуры.
        *temperature = dht11_dat[2] + (double)dht11_dat[3] / divisorForDouble(dht11_dat[3]);
        pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
        return true;
    }
    else
        return *(bool*)handleError("error while check control sum while read data from dht11.", true, NULL, 1, NULL, 0, NULL);
}

// Чтение данных с датчика.
RASPBERRY_SENSORS* readData() {

    pthread_self() == thread1 ? sPush(&checkStack1, "readData") : sPush(&checkStack2, "readData");

    int maxTries = 10;
    time_t current_time;                                                       // Структуры для текущего времени.
    struct tm * time_info;
    RASPBERRY_SENSORS* data = (RASPBERRY_SENSORS*)malloc(sizeof(RASPBERRY_SENSORS));

    while(readDht11Data(&data->temperature, &data->humidity) != true && maxTries-- > 0) // Чтение данных с датчика температуры и влажности.
        delay(1000);
    if(maxTries == 0) {
        free(data);
        return handleError("error while read data from dht11.", true, NULL, 0, NULL, 0, NULL);
    }
    data->hasLoud = digitalRead(MICROPIN);                                     // Чтение данных с микрофона.

    time(&current_time);                                                 // Чтение текущего времени.
    data->dateTime = asctime(localtime(&current_time));
    data->dateTime[stringLength(data->dateTime) - 1] = '\0';

    pthread_self() == thread1 ? sPop(&checkStack1) : sPop(&checkStack2);
    return data;
}