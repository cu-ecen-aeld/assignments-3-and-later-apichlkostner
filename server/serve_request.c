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

static void sent_back_logfile(int cfd);

void *serve_request(void *args)
{
    vector buffer = vector_create(1024);

    DEBUG_LOG("---------------> serve_request start");

    if (args == NULL)
            return NULL;

    serve_data *sd = (serve_data *)args;

    while(1) {
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
                    
        logger_write(&(sd->log), buffer);

        sent_back_logfile(sd->cfd);
    }
    close(sd->cfd);
    sd->cfd = -1;
    //syslog(LOG_DEBUG, "Closed connection from %s", host);
    vector_delete(buffer);

    DEBUG_LOG("<--------------- serve_request end");

    sd->finished = true;
    
    return NULL;
}

static void sent_back_logfile(int cfd)
{
    int fd = open(LOGFILE_NAME, O_RDONLY);
    char *buf[1024];

    DEBUG_LOG("sent_back_logfile start");

    ssize_t num_read = 0;
    do {
        num_read = read(fd, buf, sizeof(buf));
        DEBUG_LOG("Read %ld bytes from %d, send to %d", num_read, fd, cfd);
        if (write(cfd, buf, num_read) != num_read)
            ERROR_LOG("Could not sent back all data");
    } while(num_read > 0);
}
