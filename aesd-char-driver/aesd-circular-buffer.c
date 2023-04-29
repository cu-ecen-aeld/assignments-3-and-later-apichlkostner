/**
 * @file aesd-circular-buffer.c
 * @brief Functions and data related to a circular buffer imlementation
 *
 * @author Dan Walkes
 * @date 2020-03-01
 * @copyright Copyright (c) 2020
 *
 */

#ifdef __KERNEL__
#include <linux/string.h>
#else
#include <string.h>
#endif

#include "debug.h"
#include "aesd-circular-buffer.h"

/**
 * @param buffer the buffer to search for corresponding offset.  Any necessary locking must be performed by caller.
 * @param char_offset the position to search for in the buffer list, describing the zero referenced
 *      character index if all buffer strings were concatenated end to end
 * @param entry_offset_byte_rtn is a pointer specifying a location to store the byte of the returned aesd_buffer_entry
 *      buffptr member corresponding to char_offset.  This value is only set when a matching char_offset is found
 *      in aesd_buffer.
 * @return the struct aesd_buffer_entry structure representing the position described by char_offset, or
 * NULL if this position is not available in the buffer (not enough data is written).
 */
struct aesd_buffer_entry *aesd_circular_buffer_find_entry_offset_for_fpos(struct aesd_circular_buffer *buffer,
            size_t char_offset, size_t *entry_offset_byte_rtn )
{
    if ((buffer->in_offs == buffer->out_offs) && !buffer->full) {
        DEBUG_LOG("Buffer empty");
        return NULL;
    }

    size_t pos = buffer->out_offs;
    size_t buffer_size = sizeof(buffer->entry) / sizeof(buffer->entry[0]);
    size_t char_pos = 0;

    if (buffer->full) {
        if (char_offset < char_pos + buffer->entry[pos].size) {
            *entry_offset_byte_rtn = char_offset - char_pos;
            DEBUG_LOG("Found char_pos=%ld char=%ld", pos, *entry_offset_byte_rtn);
            return &buffer->entry[pos];
        }
        char_pos += buffer->entry[pos].size;
        pos = (pos + 1) % buffer_size;
    }

    while (pos != buffer->in_offs) {
        if (char_offset < char_pos + buffer->entry[pos].size) {
            DEBUG_LOG("Found char_pos=%ld char=%ld", pos, *entry_offset_byte_rtn);
            *entry_offset_byte_rtn =char_offset - char_pos;
            return &buffer->entry[pos];
        }
        char_pos += buffer->entry[pos].size;
        pos = (pos + 1) % buffer_size;
    }

    return NULL;
}

/**
* Adds entry @param add_entry to @param buffer in the location specified in buffer->in_offs.
* If the buffer was already full, overwrites the oldest entry and advances buffer->out_offs to the
* new start location.
* Any necessary locking must be handled by the caller
* Any memory referenced in @param add_entry must be allocated by and/or must have a lifetime managed by the caller.
*/
void aesd_circular_buffer_add_entry(struct aesd_circular_buffer *buffer, const struct aesd_buffer_entry *add_entry)
{
    size_t buffer_size = sizeof(buffer->entry) / sizeof(buffer->entry[0]);

    buffer->entry[buffer->in_offs].buffptr = add_entry->buffptr;
    buffer->entry[buffer->in_offs].size = add_entry->size;
    buffer->in_offs = (buffer->in_offs + 1) % buffer_size;

    if (buffer->full) {
        buffer->out_offs = (buffer->out_offs + 1) % buffer_size;    
    } else {
        if (buffer->in_offs == buffer->out_offs)
            buffer->full = true;
    }

    DEBUG_LOG("Adding entry, full=%d in_offs=%d out_offs=%d", buffer->full, buffer->in_offs, buffer->out_offs);
}

/**
* Initializes the circular buffer described by @param buffer to an empty struct
*/
void aesd_circular_buffer_init(struct aesd_circular_buffer *buffer)
{
    memset(buffer,0,sizeof(struct aesd_circular_buffer));
}