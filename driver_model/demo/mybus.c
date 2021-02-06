#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#include "mydev.h"

/*匹配设备成功则返回1，失败返回0*/
static int mybus_match (struct device *dev, struct device_driver *drv)
{
	struct mydevice *pdev = container_of(dev, struct mydevice, dev);

	return !strncmp(drv->name, pdev->name, strlen(drv->name));		
}

static struct bus_type mybus = {
	.name  = "myplatform",
	.match = mybus_match
};
EXPORT_SYMBOL_GPL(mybus);

module_driver(mybus, bus_register, bus_unregister);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
