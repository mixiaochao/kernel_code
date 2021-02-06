#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/miscdevice.h>
#include <linux/interrupt.h>
#include <linux/timer.h>
#include <linux/sched.h>

#include <linux/wait.h>

/*四个按键看作一个设备，设备文件名为/dev/keys*/
#define DEVNAME "keys"

/*四个按键连接到exynos4412的GPX3[2..5], EINT26, EINT27, EINT28, EINT29*/
static struct key_t {
	int num;
	int irq;
	char irqname[SZ_16];
	int cnt;
}keys[4] = {
	{ 0, IRQ_EINT(26),	"key1", 0},
	{ 1, IRQ_EINT(27),	"key2", 0},
	{ 2, IRQ_EINT(28),	"key3", 0},
	{ 3, IRQ_EINT(29),	"key4", 0},
};

/*tasklet机制: 一个中断的下半部任务对应如下一个变量*/
static struct tasklet_struct task;

/*内核空间保存4个按键的按下抬起状态*/
static char kbuf[4] = { 0 };

/*内核定时器用于消除按键抖动*/
static struct timer_list timer;

/*此变量用于标记按键被按下或抬起动作的发生*/
static int key_dnup_flag = 0;

/*实现进程阻塞*/
static wait_queue_head_t wait;

/*中断处理的下半部*/
static void do_bh_key(unsigned long data)
{
	struct key_t *pdev = (void *)data;
	int idx = pdev->num;

	pdev->cnt++;

	kbuf[idx] = pdev->cnt%2;

	/*按键的按下抬起状态更新到kbuf后，更新标记唤醒读进程*/
	key_dnup_flag = 1;

	wake_up_interruptible(&wait);
}

static void free_irq_key(void)
{
	int i;

	for (i = 0; i < 4; i++) {
		free_irq(keys[i].irq, &keys[i]);
	}
}

/*定时器中断处理函数*/
static void do_timer_key(unsigned long data)
{
	task.data = data;
	tasklet_schedule(&task);
}

/*注册中断时注册的中断处理函数*/
static irqreturn_t do_key_irq_tp(int irq, void *dev)
{
	timer.data = (unsigned long)dev;

	/*修改或设置定时器的未来时间点并启动定时器*/
	mod_timer(&timer, jiffies + msecs_to_jiffies(30));

	return IRQ_HANDLED;
}

static int request_irq_key(void)
{
	int i;
	int ret;

	for (i = 0; i < 4; i++) {
		ret = request_irq(keys[i].irq, do_key_irq_tp, 
				   IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
					keys[i].irqname, &keys[i]);

		if (ret < 0) {
			goto error0;
		}
	}

	return 0;

error0:
	while (i--) {
		free_irq(keys[i].irq, &keys[i]);
	}

	return ret;
}

static ssize_t key_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	/*希望用户读4个字节的数据*/
	if (cnt != 4) {
		return -EINVAL;
	}	

	/*用户没有按下或抬起时则睡眠*/
	wait_event_interruptible(wait, key_dnup_flag == 1);

	if (copy_to_user(buf, kbuf , 4)) {
		return -EINVAL;
	}

	key_dnup_flag = 0;

	return cnt;
}

static struct file_operations fops = {
	.owner	=  THIS_MODULE,
	.read	=  key_read,
};

/*混杂设备驱动对象实例化*/
static struct miscdevice misc = {
	.minor	=  MISC_DYNAMIC_MINOR,
	.name	=  DEVNAME,
	.fops	=  &fops,
};

static int __init kbuf_init(void)
{
	int ret;

	/*初始化中断下半部任务对象*/
	tasklet_init(&task, do_bh_key, 0);

	/*注册4个按键的中断*/
	ret = request_irq_key();
	if (ret < 0) {
		return ret;
	}

	ret = misc_register(&misc);
	if (ret < 0) {
		goto error0;
	}

	/*设置定时器的定时中断处理函数及需要的实参, 实参应该传递操作按键的结构体变量地址*/
	/*定时器需要在按键的中断处理函数中进行启动*/
	setup_timer(&timer, do_timer_key, 0);

	/*初始化用于实现进程阻塞的变量*/
	init_waitqueue_head(&wait);

	return 0;
error0:
	free_irq_key();

	return ret;

}
module_init(kbuf_init);


static void __exit kbuf_exit(void)
{
	free_irq_key();
	tasklet_kill(&task);
	del_timer_sync(&timer);
	misc_deregister(&misc);
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
