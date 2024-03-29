#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json.h"

int main() {

    // test jsons

    JSON_NODE* temp = parseStringToJson("{\"name\":\"Andrey\",\"age\":19,\"nums\":[1,2,3,4],\"school\":{\"name\":\"161\",\"status\":\"bad\"},\"enable\":\"false\"}");

    printJsonStructured(temp);
    printf("\n");

    char* array[] = {"cat", "dog", "bird"};
    addToObjectToNPlace(&temp, createArrayJsonNode(createStringArray(array, 3), "animals"), -1);

    updateJsonNodeToNumber(&temp->next, 20);
    printJsonUnstructured(temp);

    removeJson(&temp);

    return 0;
}

// to read data from raspberry

// FILE* openFile(char* fname) {
//     FILE* f = fopen(fname, "r");
//     if(f == NULL) {
//         printf("Error while open file %s.\n", fname);
//         exit(0);
//     }

//     return f;
// }

// int main(int argc, char* argv[]) {

//     printf("Processor ");

//     FILE* f = openFile("/proc/cpuinfo");

//     char* temp = (char*)malloc(80);

//     fgets(temp, 80, f);
//     fgets(temp, 80, f);

//     printf("%s", temp);

//     fclose(f);

//     printf("Min frequensy: ");

//     f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_min_freq");

//     fgets(temp, 80, f);
//     temp[strlen(temp) - 1] = '\0';

//     printf("%s Hz\n", temp);

//     fclose(f);

//     printf("Max frequensy: ");

//     f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_max_freq");

//     fgets(temp, 80, f);
//     temp[strlen(temp) - 1] = '\0';

//     printf("%s Hz\n", temp);

//     fclose(f);

//     printf("Current frequensy: ");

//     f = openFile("/sys/devices/system/cpu/cpu0/cpufreq/scaling_cur_freq");

//     fgets(temp, 80, f);
//     temp[strlen(temp) - 1] = '\0';

//     printf("%s Hz\n", temp);

//     fclose(f);

//     printf("Current temperature: ");

//     f = openFile("/sys/class/thermal/thermal_zone0/temp");

//     fgets(temp, 80, f);

//     printf("%d C\n", atoi(temp)/1000);

//     fclose(f);

//     f = openFile("/proc/meminfo");

//     fgets(temp, 80, f);

//     printf("%s", temp);

//     fgets(temp, 80, f);
//     fgets(temp, 80, f);

//     printf("%s", temp);

//     fclose(f);

//     f = openFile("/proc/uptime");

//     fgets(temp, 80, f);

//     printf("Work time: ");

//     for(int i = 0; temp[i]!=' '; i++)
//         printf("%c", temp[i]);

//     printf("\n");

//     fclose(f);

//     free(temp);

//     return 0;
// }