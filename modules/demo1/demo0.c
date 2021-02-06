#include <linux/init.h>
#include <linux/module.h>

static int demo_init(void)
{
	printk(KERN_WARNING"hello, world!\n");

	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
	printk(KERN_NOTICE"goold, world!\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
