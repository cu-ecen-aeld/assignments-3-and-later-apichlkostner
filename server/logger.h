#pragma once

#include "vector.h"
#include <pthread.h>

typedef struct logger {
    int fd;
    pthread_mutex_t mtx;
} logger;

int logger_open(logger *log, char* filename);
int logger_write(logger *log, vector buffer);
int logger_send(logger *log, int cfd);
int logger_send_seek(logger *log, int cfd, size_t x0, size_t x1);
int logger_close(logger *log);