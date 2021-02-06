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

static struct mydevice mydev3 = {
	.dev = {
		.init_name 	= "spring3",
		.bus		= &mybus,
		.release	= devrelease,
	},
	.name = "spring3",
};

static int __init mydev3_init(void)
{
	return device_register(&mydev3.dev);
}
module_init(mydev3_init);

static void __exit mydev3_exit(void)
{
	return device_unregister(&mydev3.dev);
}
module_exit(mydev3_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
