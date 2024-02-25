#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/mutex.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Latortuga0x71");
MODULE_DESCRIPTION("Lancha");
MODULE_VERSION("0.01");

#define DEVICE_NAME "lancha"
#define CLASS_NAME "lancha"

static DEFINE_MUTEX lancha_mutex;

static int LANCHA_MAJOR_NUMBER;
static struct class *LANCHA_DEVICE_CLASS;
static struct device *LANCHA_CHAR_DEVICE;

// file operation handlers
static int     lancha_open( struct inode *, struct file * );
static int     lancha_close(struct inode *, struct file * );


// file operations struct
struct file_operations lancha_fops = {
        .owner   =       THIS_MODULE,
        .open    =       lancha_open,
        .release =       lancha_close,
};

// https://www.derekmolloy.ie/writing-a-linux-kernel-module-part-2-a-character-device/
// https://www.derekmolloy.ie/kernel-gpio-programming-buttons-and-leds/
static int __init lancha_init(void){
    /// init mutex
    mutex_init(&lancha_mutex);
    LANCHA_MAJOR_NUMBER = register_chrdev(0,DEVICE_NAME,&lancha_fops);
    if (LANCHA_MAJOR_NUMBER < 0){
	 printk(KERN_INFO "[-] register chrdev failed\n");
	 return -1;
    }
    LANCHA_DEVICE_CLASS = class_create(CLASS_NAME);
    if (IS_ERR(LANCHA_DEVICE_CLASS)){
        printk(KERN_INFO "[-] create class failed\n");
	unregister_chrdev(LANCHA_MAJOR_NUMBER,DEVICE_NAME);
        return -1;
    }
    LANCHA_CHAR_DEVICE = device_create(LANCHA_DEVICE_CLASS, NULL, MKDEV(LANCHA_MAJOR_NUMBER,0),NULL,DEVICE_NAME);
    if (IS_ERR(LANCHA_CHAR_DEVICE)){
        class_destroy(LANCHA_DEVICE_CLASS);
	unregister_chrdev(LANCHA_MAJOR_NUMBER,DEVICE_NAME);
        printk(KERN_INFO "[-] create device failed\n");
        return -1;
    }
    printk(KERN_INFO "[+] Loaded Lancha\n");
    return 0;
}

static void __exit lancha_exit(void){
    mutex_unlock(&lancha_mutex);
    mutex_destroy(&lancha_mutex);
    device_destroy(LANCHA_DEVICE_CLASS,MKDEV(LANCHA_MAJOR_NUMBER,0));
    class_unregister(LANCHA_DEVICE_CLASS);
    class_destroy(LANCHA_DEVICE_CLASS);
    unregister_chrdev(LANCHA_MAJOR_NUMBER,DEVICE_NAME);
    printk(KERN_INFO "[-] Unloaded Lancha\n");
}

static int lancha_open(struct inode *inod,struct file* f){
    int major;
    int minor;
    // if (!mutex_trylock(&lancha_mutex)){mutex locked}
    major = imajor(inod);
    minor = iminor(inod);
    printk(KERN_INFO "im being opened at %d %d\n",major,minor);
    return 0;
}

static int lancha_close(struct inode *inod, struct file* f){
    int major;
    int minor;
    major = imajor(inod);
    minor = iminor(inod);
    printk(KERN_INFO "im being closed at %d %d\n",major,minor);
    return 0;
}


module_init(lancha_init);
module_exit(lancha_exit);
