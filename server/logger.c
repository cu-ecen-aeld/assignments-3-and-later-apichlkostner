#include "logger.h"
#include "debug.h"
#include "config.h"
#include "vector.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <syslog.h>
#include <unistd.h>

int logger_open(logger *log, char* filename)
{
    (void)filename;

    int s = pthread_mutex_init(&(log->mtx), NULL);
    if (s != 0) {
        syslog(LOG_ERR, "Could not init mutex for log, error %s", strerror(s));
        return -1;
    }
    DEBUG_LOG("Mutex initialized");

    return 0;
}

int logger_write(logger *log, vector buffer)
{    
    int s = pthread_mutex_lock(&(log->mtx));
    if (s != 0) {
        syslog(LOG_ERR, "Could not lock mutex: %s", strerror(s));
        return -1;
    }

    int fd = open(LOGFILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return -1;

    size_t numwr = write(fd, buffer.data, buffer.pos);
    DEBUG_LOG("logger_write: written %ld bytes", numwr);
    if (numwr != buffer.pos) {
        syslog(LOG_ERR, "Could not write to logfile");
        return -1;
    }

    fsync(fd);

    close(fd);

    s = pthread_mutex_unlock(&(log->mtx));
    if (s != 0) {
        syslog(LOG_ERR, "Could not unlock mutex: %s", strerror(s));
        return -1;
    }

    return 0;
}

int logger_send(logger *log, int cfd)
{
    DEBUG_LOG("logger readln");
    int retval = 0;

    vector buffer = vector_create(1024);

    
    int s = pthread_mutex_lock(&(log->mtx));
    
    if (s != 0) {
        syslog(LOG_ERR, "Could not lock mutex: %s", strerror(s));
        return -1;
    }

    int fd = open(LOGFILE_NAME, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
        return -1;

    off_t new_pos = lseek(fd, 0, SEEK_SET);

    if (new_pos == (off_t)-1) {
        syslog(LOG_ERR, "Could not set file position for reading: %s", strerror(errno));
        syslog(LOG_DEBUG, "Would be %ld", new_pos);
        return -1;
    } else {
        syslog(LOG_DEBUG, "Setting pos in logfile to %ld", new_pos);
    }
    ssize_t num_read = 0;

    do {
        num_read = read(fd, buffer.data, buffer.size);
        DEBUG_LOG("logger_send read %ld bytes from %d, send to %d", num_read, fd, cfd);

        if (num_read > 0) {
            if (write(cfd, buffer.data, num_read) != num_read)
                ERROR_LOG("Could not sent back all data");
        }
    } while(num_read > 0);

    close(fd);

    s = pthread_mutex_unlock(&(log->mtx));
    if (s != 0) {
        syslog(LOG_ERR, "Could not unlock mutex: %s", strerror(s));
        retval = -1;
    }

    vector_delete(&buffer);
    return retval;
}

int logger_close(logger *log)
{
    (void)log;
    return 0;
}