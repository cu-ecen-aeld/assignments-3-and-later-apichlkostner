#include "signalhandler.h"
#include "debug.h"
#include "server.h"

static void signal_handler(int sig);


int install_signalhandler()
{
    if ((signal(SIGINT, signal_handler) == SIG_ERR) ||
        (signal(SIGTERM, signal_handler) == SIG_ERR))
        return -1;
    else
        return 0;
}

static void signal_handler(int sig)
{
    DEBUG_LOG("Signalhandler");
    if ((sig == SIGINT) || (sig == SIGTERM))
        server_stop();
    else
        ERROR_LOG("Signalhandler called with unexpected signal");
}
