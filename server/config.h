#pragma once

#define PORT_NUM 9000
#define BACKLOG 50
#define BUFFERSIZE 1024

#define USE_AESD_CHAR_DEVICE
#ifdef USE_AESD_CHAR_DEVICE
#define LOGFILE_NAME "/dev/aesdchar"
#else
#define LOGFILE_NAME "/var/tmp/aesdsocketdata"
#endif