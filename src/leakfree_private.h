//
// Created by 方昱恒 on 2023/3/1.
//

#ifndef LEAKFREE_LEAKFREE_PRIVATE_H
#define LEAKFREE_LEAKFREE_PRIVATE_H

#include <stdio.h>
#include "uthash.h"

#define private
#define internal
#define public

typedef struct codeLocation {
    const char *file;
    const char *function;
    int line;
} CodeLocation;

typedef struct allocated {
    void *addr;                 /* Need To Free Map Key */
    size_t size;
    size_t allocateCount;
    CodeLocation codeLocation;
    char *codeLocationStr;      /* Allocated Map Key */
    UT_hash_handle hh;
} Allocated;

char *locationStringComposeWithMalloc(CodeLocation location);

#endif //LEAKFREE_LEAKFREE_PRIVATE_H
