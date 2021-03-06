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

#include <linux/input.h>

/*四个按键连接到exynos4412的GPX3[2..5], EINT26, EINT27, EINT28, EINT29*/
static struct key_t {
	int irq;
	char irqname[SZ_16];
	int cnt;
	int code;
}keys[4] = {
	{IRQ_EINT(26),	"key1", 0, KEY_LEFT},
	{IRQ_EINT(27),	"key2", 0, KEY_RIGHT},
	{IRQ_EINT(28),	"key3", 0, KEY_SPACE},
	{IRQ_EINT(29),	"key4", 0, KEY_ESC},
};

/*
 * 四个按键看成一个input设备
 * 定义一个代表input设备的结构体指针
 */
//2nd
static struct input_dev *keydev = NULL;

/*tasklet机制: 一个中断的下半部任务对应如下一个变量*/
static struct tasklet_struct task;

/*内核定时器用于消除按键抖动*/
static struct timer_list timer;

/*中断处理的下半部*/
static void do_bh_key(unsigned long data)
{
	struct key_t *pdev = (void *)data;
	pdev->cnt++;

	/*向内核的input核心层上报按键事件的数据*/
	input_report_key(keydev, pdev->code, pdev->cnt%2);
	input_sync(keydev);
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

static int __init keys_init(void)
{
	int error;
	int i;

	/*3rd: 为input设备分配结构体空间*/
	keydev = input_allocate_device();
	if (!keydev) {
		error = -ENOMEM;
		return error;
	}

	/*4th: 设置事件分类和分类的编码*/

	/*4个按键产生的数据属于按键事件分类: EV_KEY*/
	set_bit(EV_KEY, keydev->evbit);

	for (i = 0; i < ARRAY_SIZE(keys); i++) {
		set_bit(keys[i].code, keydev->keybit);
	}

	/*5th: 注册输入设备驱动*/
	error = input_register_device(keydev);
	if (error) {
		printk(KERN_ERR "input_register_device error.\n");
		goto error0;
	}

	/*初始化中断下半部任务对象*/
	tasklet_init(&task, do_bh_key, 0);

	/*注册4个按键的中断*/
	error = request_irq_key();
	if (error < 0) {
		printk(KERN_ERR "request key irq failure.\n");
		goto error1;
	}

	/*设置定时器的定时中断处理函数及需要的实参, 实参应该传递操作按键的结构体变量地址*/
	/*定时器需要在按键的中断处理函数中进行启动*/
	setup_timer(&timer, do_timer_key, 0);

	return 0;
error1:
	input_unregister_device(keydev);

	return error;
error0:
	input_free_device(keydev);

	return error;

}
module_init(keys_init);


static void __exit keys_exit(void)
{
	free_irq_key();
	tasklet_kill(&task);
	del_timer_sync(&timer);
	input_unregister_device(keydev);
}
module_exit(keys_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
