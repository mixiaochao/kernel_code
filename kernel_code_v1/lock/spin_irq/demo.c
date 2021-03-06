#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>

/*以上头文件所在的路劲为include/linux/...*/

extern void irq_keys_unregister(void);
extern int irq_keys_register(void);

#define DEVNAME "spin"

int kcnt = 1;

#define CNT 5

/*实例化的自旋锁对象*/
static spinlock_t spin;

static int  demo_open (struct inode *inodp, struct file *filp)
{
	return 0;
}

void print_kcnt_str(const char *str)
{
	int i;
	unsigned long flag;

	/*获得自旋锁， 并保存cpsr寄存器的值到flag变量，设置CPSR的第I位为1*/
	spin_lock_irqsave(&spin, flag);
	for (i = 0; i < CNT; i++) {
		printk("%s	-->   %d\n", str, kcnt);
		kcnt++;
		mdelay(50);
	}
	spin_unlock_irqrestore(&spin, flag);
}

static long demo_unlocked_ioctl (struct file *filp, unsigned int request, 
								unsigned long arg)
{
	print_kcnt_str((char *)arg);

	return 0;
}

static int  demo_release (struct inode *inodp, struct file *filp)
{
	return 0;
}

static struct file_operations fops = {
	.open		=	demo_open,
	.unlocked_ioctl	=	demo_unlocked_ioctl,
	.release	=	demo_release,
};

static struct miscdevice  misc = {
	.minor	=  MISC_DYNAMIC_MINOR, /*动态由内核分配一个次设备号， 主设备号默认为10*/
	.name	=  DEVNAME, /*指定设备文件名*/
	.fops	=  &fops, /*指定和设备文件关联的驱动集合*/
};

static int __init demo_init(void)
{
	int ret;

	/*自旋锁的初始化*/
	spin_lock_init(&spin);
	
	ret = irq_keys_register();
	if (ret < 0) {
		return ret;
	}

	/*混杂设备驱动的注册方法, 注册一个字符设备驱动*/
	ret = misc_register(&misc);
	if (ret < 0) {
		irq_keys_unregister();
		return ret;
	}

	return 0;
}

static void __exit demo_exit(void)
{
	misc_deregister(&misc);
	irq_keys_unregister();
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_AUTHOR("millet9527");
MODULE_VERSION("mi plus 2017");
MODULE_DESCRIPTION("It is a demo for driver module");
MODULE_LICENSE("GPL");
