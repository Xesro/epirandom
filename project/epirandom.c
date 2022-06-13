#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/irq.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/random.h>
#include <linux/uaccess.h>

/*  Prototypes - this would normally go in a .h file */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "epirandom" /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80 /* Max length of the message from the device */

static const char    g_s_Hello_World_string[] = "Hello world from kernel mode!\n\0";
static const ssize_t g_s_Hello_World_size = sizeof(g_s_Hello_World_string);
/* Global variables are declared as static, so are global within the file. */

static int major; /* major number assigned to our device driver */

enum {
    CDEV_NOT_USED = 0,
    CDEV_EXCLUSIVE_OPEN = 1,
};

/* Is device open? Used to prevent multiple access to device */
static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);

static char msg[BUF_LEN]; /* The msg the device will give when asked */

static struct class *cls;

static struct file_operations chardev_fops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release,
};

static int __init chardev_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &chardev_fops);

    if (major < 0) {
        pr_alert("Registering char device failed with %d\n", major);
        return major;
    }

    pr_info("I was assigned major number %d.\n", major);

    cls = class_create(THIS_MODULE, DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major, 0), NULL, DEVICE_NAME);

    pr_info("Device created on /dev/%s\n", DEVICE_NAME);

    return SUCCESS;
}

static void __exit chardev_exit(void)
{
    device_destroy(cls, MKDEV(major, 0));
    class_destroy(cls);

    /* Unregister the device */
    unregister_chrdev(major, DEVICE_NAME);
}

/* Methods */

/* Called when a process tries to open the device file, like
 * "sudo cat /dev/chardev"
 */
static int device_open(struct inode *inode, struct file *file)
{
//    static int counter = 0;

    if (atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))
        return -EBUSY;

//    sprintf(msg, "I already told you %d times Hello world!\n", counter++);
    try_module_get(THIS_MODULE);

    return SUCCESS;
}

/* Called when a process closes the device file. */
static int device_release(struct inode *inode, struct file *file)
{
    /* We're now ready for our next caller */
    atomic_set(&already_open, CDEV_NOT_USED);

    /* Decrement the usage count, or else once you opened the file, you will
     * never get rid of the module.
     */
    module_put(THIS_MODULE);

    return SUCCESS;
}

/* Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h   */
                           char __user *buffer, /* buffer to fill with data */
                            size_t length, /* length of the buffer     */
                            loff_t *offset
                            )
{
/*
    int bufferSize = 100;
    char writeBuffer[bufferSize];

    //    while(1) {
    get_random_bytes(writeBuffer, bufferSize);
    length = 100;

    if(get_user(filp, buffer) != 0) {
    pr_alert("Error while writing in kernel space.\n");
    return 0;
    }
    filp + 100;
//    }
    int bytes_read = 0;
    const char *msg_ptr = msg;
    if (!*(msg_ptr + *offset)) {
        *offset = 0;
        return 0;
    }

msg_ptr += *offset;
    while (length && *msg_ptr) {
        put_user(*(msg_ptr++), buffer++);
        length--;
        bytes_read++;
    }
//
*offset += bytes_read;



return bytes_read;
*/
    printk( KERN_NOTICE "Simple-driver: Device file is read at offset = %i, read bytes count = %u\n"
    , (int)*offset
    , (unsigned int)length );
    /* If position is behind the end of a file we have nothing to read */
//    if( *offset >= g_s_Hello_World_size )
//    return 0;
    /* If a user tries to read more than we have, read only as many bytes as we have */
//    if( *offset + length > g_s_Hello_World_size )
//    length = g_s_Hello_World_size - *offset;
//    if( copy_to_user(buffer, g_s_Hello_World_string + *offset, length) != 0 )
////        return -EFAULT;
    int bufferSize = 100;
    char writeBuffer[bufferSize];
    int count = 0;

    while(count < bufferSize) {
        writeBuffer[count] = 50;
        count++;
    }

    //    while(1) {
    get_random_bytes(writeBuffer, bufferSize);
    if( copy_to_user(buffer, writeBuffer, bufferSize) != 0 )
        return -EFAULT;

    *offset += length;
    return bufferSize;
}

/* Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t device_write(struct file *filp, const char __user *buff,
size_t len, loff_t *off)
{
pr_alert("Sorry, this operation is not supported.\n");
return -EINVAL;
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");

