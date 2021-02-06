#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#include <linux/miscdevice.h>

#include <linux/delay.h>

#define DEVNAME "spin"

#define OUT_STRING_CNT 3

static int g_count = 0;

spinlock_t spin;

extern int request_irq_key(void);
extern void free_irq_key(void);

void print_string(const char *str)
{
	int i = 0;	
	unsigned long flags;
	
	/*加锁且禁用CPU核心对中断的响应，同时读取cpsr寄存器的值到flags变量*/
	spin_lock_irqsave(&spin, flags);

	for (i = 0; i < OUT_STRING_CNT; i++) {
		printk("\"%d\": %s\n", g_count, (char *)str);
		g_count++;
		mdelay(11);
	}
	
	/*解锁且恢复cpsr寄存器的值*/
	spin_unlock_irqrestore(&spin, flags);
}

/**
 * 上层应用调用ioctl通过第三个参数仅仅传递一个字符串的地址即可
 *
 * 设计的ioctl希望,无论如何调用，打印的连续的三次计数对应相同的字符串。
 *
 */
static long spin_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{

	print_string((char *)arg);
	

	return 0;
}

static struct file_operations fops = {
	.owner 	 	= THIS_MODULE,
	.unlocked_ioctl = spin_ioctl,
};

static struct miscdevice miscled = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int __init demo_init(void)
{
	int ret = 0;

	spin_lock_init(&spin);

	ret = request_irq_key();
	if (ret < 0) {
		return ret;
	}

	ret = misc_register(&miscled);
	if (ret < 0) {
		free_irq_key();
	}

	return ret;
}

module_init(demo_init);

static void __exit demo_exit(void)
{
	misc_deregister(&miscled);
	free_irq_key();
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
