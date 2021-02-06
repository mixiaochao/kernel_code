#include <linux/init.h>
#include <linux/module.h>

static int demo_init(void)
{
	printk("HZ = %d, jiffies = %lu, sizeof(jiffies) = %d!\n", HZ, jiffies, sizeof(jiffies));

	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
	printk("goold, world!\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
