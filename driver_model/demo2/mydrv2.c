#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#include "mydev.h"

extern struct bus_type mybus;

static int myprobe (struct device *dev)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);

	printk("In %s: %s device match ok %s driver.\n",
			__func__, pdev->name, dev->driver->name);

	return 0;
}

static int myremove (struct device *dev)
{
	printk("In %s: %s driver is removed.\n",
			__func__, dev->driver->name);

	return 0;
}

static struct device_driver mydrv2 = {
	.name	=  	"cup",
	.bus	=  	&mybus,
	.probe	=  	myprobe,
	.remove	=	myremove,
};

module_driver(mydrv2, driver_register, driver_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
