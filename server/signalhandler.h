#pragma once

#include <signal.h>

typedef void (*signalhandler_t)(int);

int install_signalhandler();