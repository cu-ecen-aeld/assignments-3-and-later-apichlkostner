
#include "server.h"
#include "debug.h"
#include "signalhandler.h"
#include <string.h>
#include <syslog.h>
#include <stddef.h>

int main (int argc, char *argv[]) 
{
    (void)argc; (void)argv;

    if (install_signalhandler() == -1)
        ERROR_LOG("Could not install signal handler");

    openlog(NULL, 0, LOG_USER);

    bool daemon = false;
    if ((argc > 1) && (strcmp(argv[1], "-d") == 0)) {
        daemon = true;
    }

    return server_run(daemon);
}
