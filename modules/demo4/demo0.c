#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#define NUM 8

static int lednum = 4;
module_param(lednum, int, 0);

static char * str = "snow...";
module_param(str, charp, 0644);

static int num = NUM;
static unsigned int ar[NUM] = {1, 2, 3, 4, 5, 6, 7, 8};
module_param_array(ar, uint, &num, 0644);

static int __init demo_init(void)
{
	int i = 0;

	for (i = 0; i < lednum; i++) {
		printk("the number of led is %d\n", lednum);
		printk("%s\n", str);
	}

	for (i = 0; i < num; i++) {
		printk("ar[%d] = %d\n", i, ar[i]);
	}

	return 0;
}

module_init(demo_init);


static void __exit demo_exit(void)
{
	printk("goodbye, world!\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
