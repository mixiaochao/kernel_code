#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static struct platform_device pltdev03 = {
	.name	=  "spring",	
	.id	= 2,
};

module_driver(pltdev03, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
