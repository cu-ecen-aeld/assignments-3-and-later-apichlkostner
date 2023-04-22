#pragma once

#include <stdio.h>

#define DEBUG

#ifndef DEBUG
    #define DEBUG_LOG(msg,...)
#else
    #define DEBUG_LOG(msg,...) printf("Server: " msg "\n" , ##__VA_ARGS__)
#endif

#define ERROR_LOG(msg,...) printf("Server ERROR: " msg "\n" , ##__VA_ARGS__)