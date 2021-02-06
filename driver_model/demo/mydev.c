#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#include "mydev.h"

extern struct bus_type mybus;

static void devrelease(struct device *dev)
{
	printk(KERN_INFO "%s device is release.\n", dev->init_name);
}

static struct mydevice mydev = {
	.dev = {
		.init_name 	= "spring0",
		.bus		= &mybus,
		.release	= devrelease,
	},
	.name = "spring0",
};

static int __init mydev_init(void)
{
	return device_register(&mydev.dev);
}
module_init(mydev_init);

static void __exit mydev_exit(void)
{
	return device_unregister(&mydev.dev);
}
module_exit(mydev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
