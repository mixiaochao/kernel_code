#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static struct platform_device pltdev02 = {
	.name	=  "spring",	
	.id	= 1,
};

module_driver(pltdev02, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
