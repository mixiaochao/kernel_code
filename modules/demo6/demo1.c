#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int glb = 9527;
EXPORT_SYMBOL_GPL(glb);

static void kernel_fool(void)
{
	printk("In %s: %s : %d, glb = %d\n", __FILE__, __func__, __LINE__, glb);
}
EXPORT_SYMBOL_GPL(kernel_fool);

static int __init demo_init1(void)
{
	kernel_fool();

	return 0;
}

module_init(demo_init1);


static void __exit demo_exit1(void)
{
	printk("goodbye, world!\n");
}

module_exit(demo_exit1);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
