/*
 * chardev.c: Creates a read-only char device that says how many times
 * you've read from the dev file
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> /* for put_user */
#include <asm/io.h>

/*
 * Prototypes - this would normally go in a .h file
 */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
#define SUCCESS 0
#define DEVICE_NAME "practica" /* Dev name as it appears in /proc/devices */
#define BUF_LEN 80 /* Max length of the message from the device */
/*
 * Global variables are declared as static, so are global within the file.
 */
static int Major; /* Major number assigned to our device driver */
static int Device_Open = 0; /* Is device open?
* Used to prevent multiple access to device */
static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/*
 * This function is called when the module is loaded
 */
int init_module(void) {
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device practica failed with %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "I was assigned major number %d. To talk to\n", Major);
    printk(KERN_INFO "the driver, create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");
    return SUCCESS;
}

/*
 * This function is called when the module is unloaded
 */
void cleanup_module(void) {
    unregister_chrdev(Major, DEVICE_NAME); /* Unregister the device */
}
/*
 * Methods
 */

/*
 * Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */
static int device_open(struct inode *inode, struct file *file) {
    if (Device_Open)
        return -EBUSY;
    Device_Open++;
    printk("Practica driver opened!\n");
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/*
 * Called when a process closes the device file.
 */
static int device_release(struct inode *inode, struct file *file) {
    Device_Open--;
    /* We're now ready for our next caller */
    /*
     * Decrement the usage count, or else once you opened the file, you'll
     * never get get rid of the module.
     */
    module_put(THIS_MODULE);
    return 0;
}

/*
 * Called when a process, which already opened the dev file, attempts to
 * read from it.
 */
static ssize_t device_read(struct file *filp, /* see include/linux/fs.h */
        char *buffer, /* buffer to fill with data */
        size_t length, /* length of the buffer */
        loff_t * offset) {
    
    return EINVAL;
}

/*
 * Called when a process writes to dev file: echo "123" > /dev/practica
 */
static ssize_t device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
    ssize_t retries = 5;
    ssize_t timeout = 1000;
    /*  
        bit 3-7: se ignoran
        bit 2: caps lock -> 10 -> 0x2
        bit 1: num lock  -> 01 -> 0x1
        bit 0: scroll lock  -> 00  -> 0x0    
    */
    unsigned char state = 0x7; // La configuración de los leds (0x04 por ejemplo)
    
    printk("Practica device_write starting...\n");
        
    outb(0xed,0x60); // Le decimos al teclado que queremos modificar los leds
    udelay(timeout);
    while (retries > 0 && inb(0x60)!=0xfa) { // esperamos al controlador
        retries--;
        printk("Practica device_write waiting timeout...\n");
        udelay(timeout);
    }
    if (retries > 0) { // comprobamos que el teclado está listo
        outb(state,0x60);
        printk("Practica device_write write done\n");
    }
    
    printk("Practica device_write( %x ) succeded. Input length: %d\n" , state , len );
    
    return len;
}
