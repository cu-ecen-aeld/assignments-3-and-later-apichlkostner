#include "vector.h"
#include <stdlib.h>

struct vector vector_create(size_t size)
{
    vector v;
    v.data = (char *)malloc(size * sizeof(char));
    v.size = size;

    return v;
}

void vector_delete(struct vector v)
{
    if (v.data)
        free(v.data);
    v.size = 0;
}

void vector_resize(struct vector v, size_t size)
{
    v.data = realloc(v.data, size);
}
