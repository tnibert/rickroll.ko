/**
 * @file    rickroll.c
 * @author  Tim Nibert
 * @date    11 August 2018
 * @version 0.1
 * @brief  A loadable kernel module to output Never Gonna Give You Up by Rick Astley
current issues:
nothing displayed when reading from device
open with cat
read with dd if=/dev/ricky
*/

#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define DEVICE_NAME "ricky"
#define CLASS_NAME "rickroll"

MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("DrZ");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A kernel module to save the world.");  ///< The description -- see modinfo
MODULE_VERSION("0.1");              ///< The version of the module

static int majorNumber;

static struct class* rickClass = NULL;
static struct device* rickDevice = NULL;

// prototype functions for character driver
static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);

static struct file_operations fops =
{
    .open = dev_open,
    .read = dev_read,
    .release = dev_release,
};

static char *song = "Never gonna give you up";
//static short songlen = 3;  // default, maybe - strlen(song);
static int numberOpens = 0;

/** @brief The LKM initialization function
 *  The static keyword restricts the visibility of the function to within this C file. The __init
 *  macro means that for a built-in driver (not a LKM) the function is only used at initialization
 *  time and that it can be discarded and its memory freed up after that point.
 *  @return returns 0 if successful
 */
static int __init ricky_init(void) {
    printk(KERN_INFO "He's never gonna give you up\n");
    
    // dynamically allocate major number for device
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if(majorNumber < 0) {
        printk(KERN_ALERT "Ricky failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "Ricky is in the building with major number %d\n", majorNumber);

    // register device class
    rickClass = class_create(THIS_MODULE, CLASS_NAME);
    if(IS_ERR(rickClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Failed to register device class for rick roll\n");
        return PTR_ERR(rickClass);
    }
    printk(KERN_INFO "Ricky has a device class\n");

    // register the device driver
    rickDevice = device_create(rickClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if(IS_ERR(rickDevice)) {
        class_destroy(rickClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "Rick Astley could not create the device\n");
        return PTR_ERR(rickDevice);
    }

    printk(KERN_INFO "Ricky has a device drive registered and is ready to serenade you!\n");

    return 0;
}

/** @brief The LKM cleanup function
 *  Similar to the initialization function, it is static. The __exit macro notifies that if this
 *  code is used for a built-in driver (not a LKM) that this function is not required.
 */
static void __exit ricky_exit(void){
    device_destroy(rickClass, MKDEV(majorNumber, 0));   // remove device
    class_unregister(rickClass);                        // unregister device class
    class_destroy(rickClass);                           // remove device class
    unregister_chrdev(majorNumber, DEVICE_NAME);        // unregister major number

    printk(KERN_INFO "Rick Astley exits stage left\n");
}

static int dev_open(struct inode *inodep, struct file *filep) {
    numberOpens++;
    printk(KERN_INFO "Rick roll device has been opened %d time(s)\n", numberOpens);
    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
    int error_count = 0;
    error_count = copy_to_user(buffer, song, strlen(song));

    if(error_count == 0) {
        printk(KERN_INFO "Ricky sang to the user\n");
        return 0;
    }
    else {
        printk(KERN_INFO "Ricky wasn't able to siiiing, failed to send %d characters\n", error_count);
        return -EFAULT;
    }
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "Rick roll device successfully closed\n");
    return 0;
}

/** @brief A module must use the module_init() module_exit() macros from linux/init.h, which
 *  identify the initialization function at insertion time and the cleanup function (as
 *  listed above)
 */
module_init(ricky_init);
module_exit(ricky_exit);
