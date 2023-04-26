#include "server.h"
#include "config.h"
#include "debug.h"
#include "damon.h"
#include "logger.h"
#include "readline.h"
#include "serve_request.h"
#include "vector.h"

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
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/queue.h>



static bool server_running = true;
static int sfd;

#define RETURN_FROM_LOOP_WITH_ERROR {server_running = false; ret_val = -1; break;}

static void serve_data_init(serve_data *sd, int cfd, logger log);

int server_run(bool daemon)
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT_NUM);

    sfd = socket(AF_INET, SOCK_STREAM, 0);
    int cfd = -1;

    if (sfd == -1)
        return -1;

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        return -1;

    DEBUG_LOG("Bind");
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) {
        ERROR_LOG("Could not bind to socket: %s", strerror(errno));
        return -1;
    }

    if (daemon) {
        DEBUG_LOG("Call damonize");
        if (daemonize() == -1)
            ERROR_LOG("Error trying to damonize");
    }

    DEBUG_LOG("Listen");
    if (listen(sfd, BACKLOG) == -1) {
        ERROR_LOG("Could not listen to socket: %s", strerror(errno));
        return -1;
    }

    int ret_val = 0;

    logger log;

    int ret = logger_open(&log, LOGFILE_NAME);

    if (ret == -1) {
        syslog(LOG_ERR, "Could not open logfile %s, error %s", LOGFILE_NAME, strerror(errno));
        return -1;
    }

    serve_data sd[100];
    size_t thread_num = 0;

    serve_data_init(&sd[thread_num], cfd, log);
    int s = pthread_create(&sd[thread_num].thread, NULL, timelog, (void *)&sd[thread_num]);
    thread_num++;
    if (s != 0)
        return -1;

    while(server_running) {
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        struct sockaddr_storage claddr;
        
        DEBUG_LOG("Accept");

        cfd = accept(sfd, (struct sockaddr *)&claddr, &addrlen);

        if (cfd == -1)
            RETURN_FROM_LOOP_WITH_ERROR;

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        if (getnameinfo((struct sockaddr *)&claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) != 0)
            snprintf(host, NI_MAXHOST, "UNKNOWN");

        syslog(LOG_DEBUG, "Accepted connection from %s", host);
        DEBUG_LOG("ADDR = %s", host);

        // serve request with new thread
        serve_data_init(&sd[thread_num], cfd, log);
        int s = pthread_create(&sd[thread_num].thread, NULL, serve_request, (void *)&sd[thread_num]);
        thread_num++;
        if (s != 0)
            return -1;
        
        // join threads which have finished
        for (size_t i = 0; i < thread_num; i++) {
            if (sd[i].finished && !sd[i].joined) {
                int s = pthread_join(sd[i].thread, NULL);
                if (s != 0)
                    return -1;
                sd[i].joined = true;
            }
        }
    }

    // send signal to thread to terminate
    for (size_t i = 0; i < thread_num; i++) {
        if (!sd[i].finished) {
            sd[i].do_exit = true;
            pthread_kill(sd[i].thread, SIGTERM);
        }
    }
    // join threads
    for (size_t i = 0; i < thread_num; i++) {
        if (!sd[i].joined) {
            int s = pthread_join(sd[i].thread, NULL);
            if (s != 0)
                return -1;
            sd[i].joined = true;
        }
    }

    // clean file descriptors etc.
    if (sfd != -1)
        close(sfd);
    
    logger_close(&log);

    if (cfd != -1)
        close(cfd);

    if ((access(LOGFILE_NAME, F_OK) == 0) && remove(LOGFILE_NAME) == -1) {
        ERROR_LOG("Error removing logile: %s", strerror(errno));
        ret_val = -1;
    }

    return ret_val;
}

void server_stop()
{
    server_running = false;
    int old_errno = errno;
    if (shutdown(sfd, SHUT_RDWR) == -1)
        ERROR_LOG("Error shutdown socket: %s", strerror(errno));
    errno = old_errno;
}

static void serve_data_init(serve_data *sd, int cfd, logger log)
{
    sd->cfd = cfd;
    sd->log = log;
    sd->finished = false;
    sd->joined = false;
    sd->do_exit = false;
}


