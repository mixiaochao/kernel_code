#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int print_debug_init(void)
{
	printk("In %s string is : %s\n", __func__, CONFIG_OURSTR);

	return 0;
}

static void print_debug_exit(void)
{
	printk("goodbye...");	
}


module_init(print_debug_init);
module_exit(print_debug_exit);

MODULE_LICENSE("GPL");
