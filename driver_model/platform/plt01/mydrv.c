#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static int myprobe(struct platform_device *pdev)
{
	printk("%s device match ok %s driver...\n",
			pdev->name, pdev->dev.driver->name);

	return 0;	
}

static int myremove(struct platform_device *pdev)
{
	printk("%s driver is removed...\n",
			pdev->dev.driver->name);
	return 0;
}

static struct platform_driver pltdrv = {
	.probe	= myprobe, 
	.remove	= myremove,
	.driver	= {
		.name = "spring",		
	}
};

module_platform_driver(pltdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
