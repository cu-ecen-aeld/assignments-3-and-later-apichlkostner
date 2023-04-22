#include "damon.h"
#include "debug.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <syslog.h>

int daemonize()
{
    DEBUG_LOG("Start damonizing");
    int pid = fork();

    if (pid != 0)
        exit(EXIT_SUCCESS);
    
    DEBUG_LOG("Child after first fork");
    setsid();

    pid = fork();

    if (pid != 0)
        exit(EXIT_SUCCESS);

    DEBUG_LOG("Child after second fork");

    umask(0);
    if (chdir("/") == -1)
        return -1;

    DEBUG_LOG("Closing FDs");

    fclose(stdout);
    fclose(stderr);

    DEBUG_LOG("Daemonize return");
    return 0;
}