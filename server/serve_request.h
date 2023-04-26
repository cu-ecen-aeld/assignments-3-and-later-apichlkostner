#pragma once

#include "vector.h"
#include "logger.h"
#include <stdbool.h>

typedef struct serve_data {
    pthread_t thread;
    int cfd;
    logger log;
    bool finished;
    bool joined;
    bool do_exit;
} serve_data;

void *serve_request(void *args);

void *timelog(void *args);