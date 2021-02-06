#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>

static void pltdev01_release (struct device *dev)
{
	
}

static struct resource	srcarray[] = {
	{
		.start	= 0x110002e0,
		.end	= 0x110002e4 + 3,
		.name	= "led",
		.flags	= IORESOURCE_MEM,
	},	
	{
		.start	= IRQ_EINT(26),	
		.end	= IRQ_EINT(29),	
		.name	= "keys",
		.flags	= IORESOURCE_IRQ,
	}
};

static struct platform_device pltdev01 = {
	.name	=  "apple",	
	.id	= -1,
	.dev    = {
		.release = pltdev01_release,
	},
	.num_resources = ARRAY_SIZE(srcarray),
	.resource = srcarray,
};

module_driver(pltdev01, platform_device_register, platform_device_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
