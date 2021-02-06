#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static void pltdev02_release (struct device *dev)
{
	
}

static struct platform_device pltdev02 = {
	.name	=  "banana",	
	.id	= -1,
	.dev	= {
		.release = pltdev02_release,
	},
};

module_driver(pltdev02, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
