#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#include "mydev.h"

extern struct bus_type mybus;

static void devrelease(struct device *dev)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);

	printk(KERN_INFO "%s device is release.\n", pdev->name);
}

static struct mydevice mydev1 = {
	.dev = {
		.init_name 	= "spring1",
		.bus		= &mybus,
		.release	= devrelease,
	},
	.name = "spring1",
};

static int __init mydev1_init(void)
{
	return device_register(&mydev1.dev);
}
module_init(mydev1_init);

static void __exit mydev1_exit(void)
{
	return device_unregister(&mydev1.dev);
}
module_exit(mydev1_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
