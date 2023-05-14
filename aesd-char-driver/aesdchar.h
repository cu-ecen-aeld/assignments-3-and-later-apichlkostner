/*
 * aesdchar.h
 *
 *  Created on: Oct 23, 2019
 *      Author: Dan Walkes
 */

#ifndef AESD_CHAR_DRIVER_AESDCHAR_H_
#define AESD_CHAR_DRIVER_AESDCHAR_H_



#include "aesd-circular-buffer.h"

struct aesd_dev
{
    /**
     * TODO: Add structure(s) and locks needed to complete assignment requirements
     */
    struct aesd_circular_buffer cb;
    struct mutex lock;
    struct cdev cdev;
};


#endif /* AESD_CHAR_DRIVER_AESDCHAR_H_ */
