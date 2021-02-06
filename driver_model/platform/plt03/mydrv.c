#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>

static int myprobe(struct platform_device *pdev)
{
	struct resource *res = NULL;

	/*platform_get_irq/platform_get_irq_byname*/
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	printk("%s, %u, %u\n", res->name, res->start, res->end);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "led");
	printk("%s, %#x, %#x\n", res->name, res->start, res->end);

	return 0;	
}

static int myremove(struct platform_device *pdev)
{
	printk("%s driver is removed...\n",
			pdev->dev.driver->name);
	return 0;
}

const struct platform_device_id tables[] = {
	{"shashasha", },
	{"apple", },
	{"banana", },
	{"cherry", },
	{"", },    /*对数组的遍历结束，是通过判断最后一个元素是否是NULL*/
};

static struct platform_driver pltdrv = {
	.probe	= myprobe, 
	.remove	= myremove,
	.driver	= {
		.name = "spring",		
	},
	.id_table = tables,
};

module_platform_driver(pltdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
