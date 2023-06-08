#include "serve_request.h"
#include "config.h"
#include "debug.h"
#include "readline.h"

#include <stdbool.h>
#include <errno.h>
#include <memory.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


void *serve_request(void *args)
{
    vector buffer = vector_create(1024);

    DEBUG_LOG("---------------> serve_request start");

    if (args == NULL)
            return NULL;

    serve_data *sd = (serve_data *)args;

    while(!sd->do_exit) {
        DEBUG_LOG("Read from %d", sd->cfd);
        ssize_t num_read = 0;
        bool readline_finished = false;

        do {
            readline_finished = read_line(sd->cfd, &buffer.data[buffer.pos], buffer.size - buffer.pos, &num_read);

            buffer.pos += num_read;

            if (num_read <= 0)
                break;
                
            // increase buffer size if necessary
            if (!readline_finished) {
                DEBUG_LOG("Resizing vector");
                buffer = vector_resize(buffer, 2 * buffer.size);
            }
        } while(!readline_finished);

        if (num_read <= 0)
            break;
        
        char *seekstr = "AESDCHAR_IOCSEEKTO:";
        size_t seekstr_size = strlen(seekstr);
        bool seek = ((buffer.size > seekstr_size) &&
                     (strncmp(buffer.data, seekstr, seekstr_size) == 0));
        if (seek) {
            char *data = buffer.data + seekstr_size;
            char *next;
            size_t x0, x1;

            DEBUG_LOG("ioctl buffer.data: %s", buffer.data);
            DEBUG_LOG("ioctl str: %s", data);
            x0 = strtoul(data, &next, 10);
            x1 = strtoul(next + 1, &next, 10);
            DEBUG_LOG("ioctl: %lu, %lu", x0, x1);
            if (logger_send_seek(&(sd->log), sd->cfd, x0, x1) == -1) {
                ERROR_LOG("Could not sent back data");
                break;
            }
        } else {
            logger_write(&(sd->log), buffer);
            if (logger_send(&(sd->log), sd->cfd) == -1) {
                ERROR_LOG("Could not sent back data");
                break;
            }
        }

        
    }
    close(sd->cfd);
    sd->cfd = -1;
    //syslog(LOG_DEBUG, "Closed connection from %s", host);
    vector_delete(&buffer);

    DEBUG_LOG("<--------------- serve_request end");

    sd->finished = true;
    
    return NULL;
}

void *timelog(void *args)
{
    if (args == NULL)
            return NULL;

    serve_data *sd = (serve_data *)args;
    vector buffer = vector_create(256);
    time_t t;
    struct tm *tmp;
    
    DEBUG_LOG("-> Starting timelog");
    while(!sd->do_exit) {
        sleep(10);

        t = time(NULL);
        tmp = localtime(&t);

        if (tmp == NULL)
            continue;

        if (strftime(buffer.data, buffer.size, "timestamp: %a, %d %b %Y %T %z\n", tmp) == 0)
            continue;

        buffer.pos = strlen(buffer.data);
        logger_write(&(sd->log), buffer);
    }

    vector_delete(&buffer);
    sd->finished = true;
    DEBUG_LOG("-> Stopping timelog");
    return NULL;
}
