#pragma once

#include <stdio.h>
#include <syslog.h>

#define DEBUG

#ifndef DEBUG
    #define DEBUG_LOG(msg,...)
#else
    //#define DEBUG_LOG(msg,...) printf("Server: " msg "\n" , ##__VA_ARGS__)
    #define DEBUG_LOG(msg,...) syslog(LOG_DEBUG, "Server: " msg "\n" , ##__VA_ARGS__)
#endif

#define ERROR_LOG(msg,...) printf("Server ERROR: " msg "\n" , ##__VA_ARGS__)