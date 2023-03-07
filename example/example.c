//
// Created by 方昱恒 on 2023/3/1.
//

#include "leakfree.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *mallocLeakTest(void *argv) {
    char *test = NULL;
    int i = 0;
    while (i < 10) {
        test = (char *)malloc(10 * sizeof(char));
        if (i % 2 == 0) {
            free(test);
        }
        i++;
    }
    return NULL;
}

void startMallocLeakTest(void) {
    pthread_t mallocLeakTestThread;
    int ret = 0;
    ret = pthread_create(&mallocLeakTestThread, NULL, mallocLeakTest, NULL);
    if (ret != 0) {
        printf("mallocLeakTest start error.\n");
    }
}

void *callocLeakTest(void *argv) {
    char *test = NULL;
    int i = 0;
    while (i < 10) {
        test = (char *)calloc(10, sizeof(char));
        if (i % 2 == 0) {
            free(test);
        }
        i++;
    }
    return NULL;
}

void startCallocLeakTest(void) {
    pthread_t callocLeakTestThread;
    int ret = 0;
    ret = pthread_create(&callocLeakTestThread, NULL, callocLeakTest, NULL);
    if (ret != 0) {
        printf("callocLeakTest start error.\n");
    }
}

int main(int argc, char *argv[]) {
    startMallocLeakTest();
    startCallocLeakTest();

    sleep(1);
    printCurrentMemCondition();

    return 0;
}