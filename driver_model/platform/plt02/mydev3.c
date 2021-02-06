#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static void pltdev03_release (struct device *dev)
{
	
}

static struct platform_device pltdev03 = {
	.name	=  "cherry",	
	.id	= -1,
	.dev	= {
		.release = pltdev03_release,
	},
};

module_driver(pltdev03, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
