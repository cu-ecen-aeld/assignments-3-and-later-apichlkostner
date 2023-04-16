#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
    openlog(NULL, 0, LOG_USER);

    if (argc < 3) {
        syslog(LOG_ERR, "Wrong number of arguments");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    char *writestr = argv[2];

    syslog(LOG_DEBUG, "Writing %s to %s", writestr, filename);

    int file = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (file == -1) {
        syslog(LOG_ERR, "Could not open file %s, error %s", filename, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int size_to_write = strlen(writestr);
    int num_written = write(file, writestr, size_to_write);

    if (num_written != size_to_write) {
        syslog(LOG_ERR, "Could not write complete string %s to file %s", writestr, filename);
        exit(EXIT_FAILURE);
    }

    close(file);
}
