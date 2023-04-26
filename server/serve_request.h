#pragma once

#include "vector.h"
#include "logger.h"
#include <stdbool.h>

typedef struct serve_data {
    int cfd;
    logger log;
    bool finished;
    bool joined;
} serve_data;

void *serve_request(void *args);