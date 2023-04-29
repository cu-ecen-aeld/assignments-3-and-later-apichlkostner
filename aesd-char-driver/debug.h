#pragma once

#ifdef __KERNEL__

#define DEBUG_LOG(msg,...)
#define ERROR_LOG(msg,...)

#else

#include <stdio.h>
#include <syslog.h>

#define DEBUG

#ifndef DEBUG
    #define DEBUG_LOG(msg,...)
#else
    #define DEBUG_LOG(msg,...) printf("Circular buffer: " msg "\n" , ##__VA_ARGS__)
    //#define DEBUG_LOG(msg,...) syslog(LOG_DEBUG, "Circular buffer: " msg "\n" , ##__VA_ARGS__)
#endif

#define ERROR_LOG(msg,...) printf("Circular buffer: " msg "\n" , ##__VA_ARGS__)
//#define ERROR_LOG(msg,...) syslog(LOG_ERR, "Circular buffer: " msg "\n" , ##__VA_ARGS__)

#endif