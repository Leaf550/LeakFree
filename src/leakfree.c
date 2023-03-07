//
// Created by 方昱恒 on 2023/3/1.
//

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

#include "leakfree_private.h"

#define CODE_LOCATION_STR_LEN 320

void recordLocation(void *allocated, size_t size, const char *file, const char *function, int line);
void releaseAllocatedHashMap(void *ptr, const char *file, const char *function, int line);

Allocated *allocatedMap = NULL;
Allocated *needToFreeMap = NULL;

pthread_mutex_t LEAK_FREE_ALLOC_MAP_MUTEX = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t LEAK_FREE_FREE_MAP_MUTEX = PTHREAD_MUTEX_INITIALIZER;

public void *leakFreeMalloc(size_t size, char *file, char *function, int line) {
    void *allocated = malloc(size);
    pthread_mutex_lock(&LEAK_FREE_ALLOC_MAP_MUTEX);
    recordLocation(allocated, size, file, function, line);
    pthread_mutex_unlock(&LEAK_FREE_ALLOC_MAP_MUTEX);
    return allocated;
}

public void *leakFreeCalloc(size_t count, size_t size, char *file, char *function, int line) {
    void *allocated = calloc(count, size);
    pthread_mutex_lock(&LEAK_FREE_ALLOC_MAP_MUTEX);
    recordLocation(allocated, size * count, file, function, line);
    pthread_mutex_unlock(&LEAK_FREE_ALLOC_MAP_MUTEX);
    return allocated;
}

public void leakFreeFree(void *ptr, const char *file, const char *function, int line) {
    pthread_mutex_lock(&LEAK_FREE_FREE_MAP_MUTEX);
    releaseAllocatedHashMap(ptr, file, function, line);
    pthread_mutex_unlock(&LEAK_FREE_FREE_MAP_MUTEX);
    free(ptr);
}

public void printCurrentMemCondition(void) {
    Allocated *allocated, *tmp;
    HASH_ITER(hh, allocatedMap, allocated, tmp) {
        printf("in %s - %zu byte(s) haven't release. remain free count: %zu.\n",
               allocated->codeLocationStr, allocated->size, allocated->allocateCount);
    }
}

private void recordLocation(void *allocated, size_t size, const char *file, const char *function, int line) {
    CodeLocation location = {
            .file = file,
            .function = function,
            .line = line
    };
    char *locationStr = locationStringComposeWithMalloc(location);

    /* Record allocated */
    Allocated *allocatedStruct = NULL;
    HASH_FIND_STR(allocatedMap, locationStr, allocatedStruct);
    if (allocatedStruct == NULL) {
        allocatedStruct = (Allocated *) malloc(sizeof (Allocated));
        allocatedStruct->size = size;
        allocatedStruct->allocateCount = 1;
        allocatedStruct->codeLocation = location;
        char *recordedLocationStr = (char *)malloc(CODE_LOCATION_STR_LEN);
        sprintf(recordedLocationStr, "%s", locationStr);
        allocatedStruct->codeLocationStr = recordedLocationStr;
        HASH_ADD_STR(allocatedMap, codeLocationStr, allocatedStruct);
    } else {
        allocatedStruct->size += size;
        allocatedStruct->allocateCount++;
    }

    /* Record need to free */
    Allocated *needToFreeStruct = NULL;
    HASH_FIND_PTR(needToFreeMap, &allocated, needToFreeStruct);
    if (needToFreeStruct == NULL) {
        needToFreeStruct = (Allocated *) malloc(sizeof (Allocated));
        needToFreeStruct->addr = allocated;
        needToFreeStruct->size = size;
        needToFreeStruct->allocateCount = 1;
        char *recordedLocationStr = (char *)malloc(CODE_LOCATION_STR_LEN);
        sprintf(recordedLocationStr, "%s", locationStr);
        needToFreeStruct->codeLocationStr = recordedLocationStr;
        HASH_ADD_PTR(needToFreeMap, addr, needToFreeStruct);
    } else {
        printf("alloc error: repeat alloc for the same address.\n");
    }

    free(locationStr);
}

internal char *locationStringComposeWithMalloc(CodeLocation location) {
    char *res = (char *) malloc(CODE_LOCATION_STR_LEN);
    memset(res, 0, CODE_LOCATION_STR_LEN);
    snprintf(res, CODE_LOCATION_STR_LEN - 2 - 1, "%s:%s:%d", location.file, location.function, location.line);
    return res;
}

private void releaseAllocatedHashMap(void *ptr, const char *file, const char *function, int line) {
    Allocated *needToFreeStruct = NULL;
    HASH_FIND_PTR(needToFreeMap, &ptr, needToFreeStruct);
    if (needToFreeStruct == NULL) {
        printf("release error: needToFreeStruct not found.\n");
        return;
    }

    char *allocLocation = needToFreeStruct->codeLocationStr;
    Allocated *allocateStruct = NULL;
    HASH_FIND_STR(allocatedMap, allocLocation, allocateStruct);
    if (allocateStruct == NULL) {
        printf("release error: allocateStruct not found.\n");
        return;
    }

    allocateStruct->size -= needToFreeStruct->size;
    allocateStruct->allocateCount--;

    if (allocateStruct->size == 0 || allocateStruct->allocateCount == 0) {
        HASH_DEL(allocatedMap, allocateStruct);
    }

    HASH_DEL(needToFreeMap, needToFreeStruct);
}