/**
 * @file aesdchar.c
 * @brief Functions and data related to the AESD char driver implementation
 *
 * Based on the implementation of the "aesd" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author Dan Walkes
 * @date 2019-10-22
 * @copyright Copyright (c) 2019
 *
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>	
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include "aesdchar.h"
#include "debug.h"

int aesd_major = 0; // use dynamic major
int aesd_minor = 0;

MODULE_AUTHOR("Arthur Pichlkostner");
MODULE_LICENSE("Dual BSD/GPL");

struct aesd_dev aesd_device;

int aesd_open(struct inode *inode, struct file *filp)
{
    struct aesd_dev *dev;
    PDEBUG("open");

	dev = container_of(inode->i_cdev, struct aesd_dev, cdev);
	filp->private_data = dev; /* for other methods */

    return 0;
}

int aesd_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    /**
     * TODO: handle release
     */
    return 0;
}

ssize_t aesd_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct aesd_dev *dev;
    size_t entry_offset_byte_rtn;
    struct aesd_buffer_entry *entry;
    PDEBUG("read %zu bytes with offset %lld", count, *f_pos);

    dev = (struct aesd_dev *)(filp->private_data);

    if (mutex_lock_interruptible(&dev->lock)) {
		return -ERESTARTSYS;
    }
    
    entry = aesd_circular_buffer_find_entry_offset_for_fpos(&dev->cb, *f_pos, &entry_offset_byte_rtn);

    if (entry) {
        count = min(count, entry->size - entry_offset_byte_rtn);
        retval = count;

        PDEBUG("read from offset pos %ld with len %ld", entry_offset_byte_rtn, count);

        if (copy_to_user(buf, entry->buffptr, count)) {
            retval = -EFAULT;
            count = 0;
            goto out;
        }
    } else {
        PDEBUG("End of file");
        count = 0;
        retval = 0;
    }

out:
	mutex_unlock(&dev->lock);
    *f_pos += count;
    PDEBUG("read: return new offset %lld", *f_pos);
	return retval;
}

ssize_t aesd_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    static char *data ;
    static size_t data_size;

    struct aesd_dev *dev;
    struct aesd_buffer_entry entry;
    ssize_t retval = -ENOMEM;
    PDEBUG("write %zu bytes with offset %lld", count, *f_pos);
    
    dev = filp->private_data;

    data = krealloc(data, data_size + count, GFP_KERNEL);    

    if (!data) {
        return -ERESTARTSYS; // TODO: real value
    }

	if (mutex_lock_interruptible(&dev->lock)) {
		return -ERESTARTSYS;
    }

    if (copy_from_user(data + data_size, buf, count)) {
		retval = -EFAULT;
		goto out;
	}

    data_size += count;

    if (data[data_size -1] == '\n') {
        const char *old_data;

        entry.buffptr = data;
        entry.size = data_size;

        old_data = aesd_circular_buffer_add_entry(&dev->cb, &entry);

        if (old_data) {
            kfree(old_data);
        }

        data = NULL;
        data_size = 0;
    }

    

	*f_pos += count;
    PDEBUG("write: return new offset %lld", *f_pos);
	retval = count;
    

out:
	mutex_unlock(&dev->lock);
	return retval;
}


loff_t aesd_llseek(struct file *filp, loff_t off, int whence)
{
	loff_t newpos;

	switch(whence) {
	  case 0: /* SEEK_SET */
		newpos = off;
		break;

	  case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

	  case 2: /* SEEK_END */
		return -EINVAL;
		break;

	  default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0) return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}

struct file_operations aesd_fops = {
    .owner =    THIS_MODULE,
    .llseek =   aesd_llseek,
    .read =     aesd_read,
    .write =    aesd_write,
    .open =     aesd_open,
    .release =  aesd_release,
};

static int aesd_setup_cdev(struct aesd_dev *dev)
{
    int err, devno = MKDEV(aesd_major, aesd_minor);

    cdev_init(&dev->cdev, &aesd_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &aesd_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding aesd cdev", err);
    }
    return err;
}



int aesd_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, aesd_minor, 1,
            "aesdchar");
    aesd_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", aesd_major);
        return result;
    }
    memset(&aesd_device, 0, sizeof(struct aesd_dev));
    
    // init aesd_device
    mutex_init(&aesd_device.lock);

    result = aesd_setup_cdev(&aesd_device);
    aesd_circular_buffer_init(&aesd_device.cb);

    if (result) {
        unregister_chrdev_region(dev, 1);
    }
    return result;

}

void aesd_cleanup_module(void)
{
    dev_t devno = MKDEV(aesd_major, aesd_minor);

    cdev_del(&aesd_device.cdev);

    /**
     * TODO: cleanup AESD specific poritions here as necessary
     */

    unregister_chrdev_region(devno, 1);
}


module_init(aesd_init_module);
module_exit(aesd_cleanup_module);
