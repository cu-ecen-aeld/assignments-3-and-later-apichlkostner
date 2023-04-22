#include "server.h"
#include "config.h"
#include "debug.h"

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
#include <stdbool.h>

#include <stdlib.h>
#include <string.h>

static bool server_running = true;
static int sfd;

int server_run()
{
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(PORT_NUM);

    sfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sfd == -1)
        return -1;

    if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
        return -1;

    DEBUG_LOG("Bind");
    if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1)
        return -1;

    DEBUG_LOG("Listen");
    if (listen(sfd, BACKLOG) == -1)
        return -1;

    while(1) {
        char buffer[BUFFERSIZE];
        socklen_t addrlen = sizeof(struct sockaddr_storage);
        struct sockaddr_storage claddr;
        
        DEBUG_LOG("Accept");

        int cfd = accept(sfd, (struct sockaddr *)&claddr, &addrlen);

        if (cfd == -1)
            return -1;

        char host[NI_MAXHOST];
        char service[NI_MAXSERV];

        if (getnameinfo((struct sockaddr *)&claddr, addrlen, host, NI_MAXHOST, service, NI_MAXSERV, 0) != 0)
            snprintf(host, NI_MAXHOST, "UNKNOWN");


        syslog(LOG_DEBUG, "Accepted connection from %s", host);
        DEBUG_LOG("ADDR = %s", host);

        DEBUG_LOG("Read");
        ssize_t num_read = read(cfd, buffer, sizeof(buffer));

        if (num_read == -1) {
            if (errno == EINTR)
                continue;
            else
                return -1;
        }

        int fd = open(LOGFILE_NAME, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1) {
            syslog(LOG_ERR, "Could not open logfile %s, error %s", LOGFILE_NAME, strerror(errno));
            return -1;
        }
        
        int num_written = write(fd, buffer, num_read);

        if (num_written < num_read)
            return -1;

        close(fd);
        close(cfd);
    }

    close(sfd);

    return 0;
}

void server_stop()
{
    server_running = false;
    int old_errno = errno;
    if (shutdown(sfd, SHUT_RDWR) == -1)
        ERROR_LOG("Error shutdown socket: %s", strerror(errno));
    errno = old_errno;
}
