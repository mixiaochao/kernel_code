#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

extern void kernel_fool(void);

static int __init demo_init(void)
{
	kernel_fool();

	return 0;
}

module_init(demo_init);


static void __exit demo_exit(void)
{
	printk("goodbye, world!\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
