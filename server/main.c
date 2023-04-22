
#include "aesdsocket.h"
#include <syslog.h>
#include <stddef.h>

int main (int argc, char *argv[]) 
{
    (void)argc; (void)argv;

    openlog(NULL, 0, LOG_USER);

    return server_run();
}
