
#include "aesdsocket.h"
#include "debug.h"
#include "signalhandler.h"
#include <syslog.h>
#include <stddef.h>

int main (int argc, char *argv[]) 
{
    (void)argc; (void)argv;

    if (install_signalhandler() == -1)
        ERROR_LOG("Could not install signal handler");

    openlog(NULL, 0, LOG_USER);

    return server_run();
}
