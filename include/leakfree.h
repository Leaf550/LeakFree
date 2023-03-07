//
// Created by 方昱恒 on 2023/3/1.
//

#ifndef LEAKFREE_LEAKFREE_H
#define LEAKFREE_LEAKFREE_H

#include <stdio.h>

void *leakFreeMalloc(size_t size, const char *file, const char *function, int line);
void *leakFreeCalloc(size_t count, size_t size, const char *file, const char *function, int line);

void leakFreeFree(void *ptr, const char *file, const char *function, int line);

void printCurrentMemCondition(void);

#define malloc(size) leakFreeMalloc(size, __FILE__, __FUNCTION__, __LINE__)

#define calloc(count, size) leakFreeCalloc(count, size, __FILE__, __FUNCTION__, __LINE__)

#define free(ptr) leakFreeFree(ptr, __FILE__, __FUNCTION__, __LINE__)

#endif //LEAKFREE_LEAKFREE_H
