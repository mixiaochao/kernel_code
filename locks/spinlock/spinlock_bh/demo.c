#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#include <linux/miscdevice.h>

#include <linux/delay.h>

#include <linux/interrupt.h>

#define DEVNAME "spin"

#define OUT_STRING_CNT 3

static int g_count = 0;

static spinlock_t spin;

extern struct tasklet_struct task;
extern int request_irq_key(void);
extern void free_irq_key(void);
extern void do_bh_key(unsigned long data);

void print_string(const char *str)
{
	int i = 0;	

	spin_lock_bh(&spin);
	for (i = 0; i < OUT_STRING_CNT; i++) {
		printk("\"%d\": %s\n", g_count, (char *)str);
		g_count++;
		mdelay(11);
	}
	spin_unlock_bh(&spin);
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
	int ret;

	spin_lock_init(&spin);

	/*初始化下版本任务对象*/
	tasklet_init(&task, do_bh_key, 0);

	ret = request_irq_key();
	if (ret < 0) {
		return ret;
	}


	ret = misc_register(&miscled);
	if (ret < 0) {
		free_irq_key();

		return ret;
	}

	return 0;
}

module_init(demo_init);

static void __exit demo_exit(void)
{
	misc_deregister(&miscled);
	free_irq_key();
	tasklet_kill(&task);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
