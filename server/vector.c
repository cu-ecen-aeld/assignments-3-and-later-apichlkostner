#include "vector.h"
#include "debug.h"
#include <stdlib.h>

vector vector_create(size_t size)
{
    vector v;
    v.data = (char *)malloc(size * sizeof(char));
    v.pos = 0;
    v.size = size;

    return v;
}

void vector_delete(vector v)
{
    if (v.data) {
        free(v.data);
        v.data = NULL;
    } else {
        ERROR_LOG("Delete invalid vector");
        exit(EXIT_FAILURE);
    }
    v.size = 0;
}

vector vector_resize(vector v, size_t size)
{
    void *old_ptr = v.data;
    v.data = realloc(old_ptr, size);
    v.size = size;

    return v;
}
