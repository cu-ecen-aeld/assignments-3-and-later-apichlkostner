#pragma once

#include <memory.h>
#include <stdbool.h>
#include <sys/types.h>


bool read_line(int fd, void *buf, ssize_t size, ssize_t *size_read);
