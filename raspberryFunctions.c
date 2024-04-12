#include "raspberryFunctions.h"

static uint8_t parsePulse(int level) {
    int count = 0;

    while(digitalRead(DHTPIN) == level) {
        delayMicroseconds(1);
        if (count++ >= maxcycles)
            return -1;
    }

    return count;
}

static bool read_dht11_dat(double* temperature, double* humidity) {

    for(int i = 0; i<5; i++) {

        olddht11_dat[i] = dht11_dat[i];
        dht11_dat[i] = 0;
    }

    pinMode(DHTPIN, OUTPUT);
    digitalWrite(DHTPIN, LOW);      // init signal to dht11
    delay(18);                        // wait 18 milliseconds to ensure DHT's detection of signal
    digitalWrite(DHTPIN, HIGH);     // send that raspberry wait signal
    delayMicroseconds(40);          // wait DHT's response
    pinMode(DHTPIN, INPUT);

    if(parsePulse(LOW) == -1)
        return false;
    if(parsePulse(HIGH) == -1)
        return false;

    uint8_t cycles[80];
    for (int i = 0; i < 80; i+=2 ) {

        cycles[i] = parsePulse(LOW);
        cycles[i+1] = parsePulse(HIGH);
    }

    for(int i = 0; i<40; i++) {

        if(cycles[2*i] == -1 || cycles[2*i+1] == -1)
            return false;

        dht11_dat[i/8] <<= 1;

        if(cycles[2*i] < cycles[2*i + 1])
            dht11_dat[i/8] |= 1;
    }

    if (dht11_dat[4] == ((dht11_dat[0] + dht11_dat[1] + dht11_dat[2] + dht11_dat[3]) & 0xFF)) {
        *humidity = dht11_dat[0] + (double)dht11_dat[1] / divisorForDouble(dht11_dat[1]);
        *temperature = dht11_dat[2] + (double)dht11_dat[3] / divisorForDouble(dht11_dat[3]);
        //printf("Humidity = %d.%d %% Temperature = %d.%d C\n", dht11_dat[0], dht11_dat[1], dht11_dat[2], dht11_dat[3]);
        return true;
    }
    else
        return false;
        //printf("Humidity = %d.%d %% Temperature = %d.%d C\n", olddht11_dat[0], olddht11_dat[1], olddht11_dat[2], olddht11_dat[3]);
}

RASPBERRY_SENSORS* readData() {

    time_t current_time;
    struct tm * time_info;
    RASPBERRY_SENSORS* data = (RASPBERRY_SENSORS*)malloc(sizeof(RASPBERRY_SENSORS));

    while(!read_dht11_dat(&data->temperature, &data->humidity))
        delay(1000);
    data->hasLoud = digitalRead(MICROPIN);

    time(&current_time);
    data->dateTime = asctime(localtime(&current_time));

    return data;
}

//int main()
//{
//    if (wiringPiSetup() == -1)
//        exit(0);
//
//    pinMode(RELAYPIN, OUTPUT);
//    pinMode(MICROPIN, INPUT);
//    int times;
//
//    while(1) {
//
//        times = 5;
//        digitalWrite(RELAYPIN, LOW);
//        while (times--) {
//            delay(10000);
//            read_dht11_dat();
//        }
//
//        times = 5;
//        digitalWrite(RELAYPIN, HIGH);
//        while(times--) {
//            delay(10000);
//            read_dht11_dat();
//            printf("Has sound: %d\n", digitalRead(MICROPIN));
//        }
//    }
//
//    return(0);
//}