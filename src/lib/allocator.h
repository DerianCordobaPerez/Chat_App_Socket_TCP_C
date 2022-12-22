#include <stdio.h>
#include <stdlib.h>
#define null NULL

const void *allocator(const size_t size) {
    void *alloc;

    if((alloc = (void*)malloc(size * sizeof(void))) == null) {
        puts("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    return alloc;
}