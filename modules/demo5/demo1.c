#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

void kernel_fool(void)
{
	printk("In %s: %s : %d\n", __FILE__, __func__, __LINE__);
}
