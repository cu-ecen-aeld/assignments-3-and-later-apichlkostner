#include "readline.h"
#include "debug.h"

#include <unistd.h>
#include <errno.h>

bool read_line(int fd, void *buf, ssize_t size, ssize_t *size_read)
{
    if ((buf == NULL) || size == 0)
        return 0;

    ssize_t pos = 0;

    char* char_buf = buf;
    char one_char = 0;
    while((pos < size) && (one_char != '\n')) {
        int n = read(fd, &one_char, 1);

        if (n == 0) {
            DEBUG_LOG("read_line: no more data");
            break;
        }
        
        char_buf[pos] = one_char;
        pos++;
    }

    *size_read = pos;

    return (one_char == '\n');
}