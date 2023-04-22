#pragma once

#include <memory.h>

typedef struct vector {
    char *data;
    size_t size;
} vector;

extern vector vector_create(size_t size);
extern void vector_delete(vector v);
extern void vector_resize(vector v, size_t size);
