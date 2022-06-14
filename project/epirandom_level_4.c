#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/irq.h>

#include <linux/moduleparam.h>


#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>

#include <linux/random.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#define MAX_ALPHABET_LENGTH 128
static char alphabet[MAX_ALPHABET_LENGTH];
static int alphabetLength = 0;

//module_param(alphabetLength, unsigned, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
//MODULE_PARM_DESC(alphabetLength, "alphabet length");
module_param_string(alphabet, alphabet, MAX_ALPHABET_LENGTH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
MODULE_PARM_DESC(alphabet, "alphabet for random generation");

//static int grades[3] = {0};
//static int arg_argc = 0;
//
//module_param_array(grades, int, &arg_argc, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
//MODULE_PARM_DESC(grades, "Your grades");

static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "epirandom"

enum {
    DEVICE_NOT_USED = 0,
    DEVICE_EXCLUSIVE_OPEN = 1,
};

static int major;

static atomic_t already_open = ATOMIC_INIT(DEVICE_NOT_USED);

static struct class *cls;

static struct file_operations chardev_fops = {
        .read = device_read,
        .open = device_open,
        .release = device_release,
        .owner = THIS_MODULE,
};

static int __init chardev_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);

    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }

    pr_info("driver major number : %d.\n", major);

    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);
}

/**
 * prevent from multiple open
 */
static int device_open(struct inode *inode, struct file *file)
{
    if (atomic_cmpxchg(&already_open, DEVICE_NOT_USED, DEVICE_EXCLUSIVE_OPEN))
        return -EBUSY;

    return SUCCESS;
}

static int device_release(struct inode *inode, struct file *file)
{
    atomic_set(&already_open, DEVICE_NOT_USED);

    return SUCCESS;
}

static ssize_t device_read(struct file *filp,
                           char __user *buffer,
                            size_t length,
                            loff_t *offset
)
{
    unsigned *kernelBuffer = (unsigned *)kmalloc(length, GFP_KERNEL);
    int count = 0;

    get_random_bytes(kernelBuffer, length);

    while(count < length) {
        *(kernelBuffer + count) = alphabet[*(kernelBuffer + count) % alphabetLength];
        count++;
    }

    if(copy_to_user(buffer, kernelBuffer, length) != 0 )
        return -EFAULT;

    kfree(kernelBuffer);

    return length;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("epirandom level 4");
MODULE_AUTHOR("Arthur Plebs");
