#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/input.h> //1st

static struct tiny4412_key {
	int irqnum;
	char *name;
	int cnt;
}keys[] = {
	{IRQ_EINT(26), "key1", 0},
	{IRQ_EINT(27), "key2", 0},
	{IRQ_EINT(28), "key3", 0},
	{IRQ_EINT(29), "key4", 0}
};

/*2nd: 定义一个输入设备对应的结构体指针变量*/
static struct input_dev *mtdev;

/*定义一个struct tasklet_struct类型的一个变量，对应中断的下半部任务*/
static struct tasklet_struct task;

/*内核定时器用于实现按键的消抖动*/
static struct timer_list timer;

static int ares[3][2] = {
	{200, 300},
	{300, 400},
	{400, 440},
};

/*定时处理函数*/
static void do_timer(unsigned long data)
{
	struct tiny4412_key *pdev = (void *)data;	
	int i;
	pdev->cnt++;

	if (pdev->cnt%2) {
		for (i = 0; i < 3; i++) {
			ares[i][0]++; ares[i][0] %= 800;
			ares[i][1]++; ares[i][1] %= 480;
			input_report_abs(mtdev, ABS_MT_POSITION_X, ares[i][0]);
			input_report_abs(mtdev, ABS_MT_POSITION_Y, ares[i][1]);
			input_mt_sync(mtdev);
		}
		input_report_abs(mtdev, ABS_MT_PRESSURE, 1);
		input_report_key(mtdev, BTN_TOUCH, 1);
	}  else {
		input_report_abs(mtdev, ABS_MT_PRESSURE, 0);
		input_report_key(mtdev, BTN_TOUCH, 0);
	}

	input_sync(mtdev);
}

/*中断下半部处理函数*/
static void do_4key_bh(unsigned long data)
{
	mod_timer(&timer, jiffies + msecs_to_jiffies(30));
	timer.data = data;
}

/*上半部处理函数： 中断所注册的中断处理函数，中断发生后由内核自动调用*/
static irqreturn_t do_4keys(int irqnum, void *data)
{
	/*给下半部任务对象的处理函数的点心变量赋值*/
	task.data = (unsigned long)data;

	/*将中断下半部任务对象交给调度器调度*/
	tasklet_schedule(&task);

	return IRQ_HANDLED;
}

static int register_4keys(void)
{
	int i = 0;
	int ret;

	for (; i < ARRAY_SIZE(keys); i++) {
		ret = request_irq(keys[i].irqnum, do_4keys, 
				IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
				keys[i].name, keys+i
		      );
		if (ret < 0) {
			goto error;
		}
	}


	return 0;
error:
	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
	}

	return ret;
}

static void unregister_4keys(void)
{
	int i = ARRAY_SIZE(keys);

	while (i--) {
		free_irq(keys[i].irqnum, keys+i);
	}
}

static int __init demo_init(void)
{
	int ret;

	/*3rd: 为输入设备分配空间*/
	mtdev = input_allocate_device();
	if (!mtdev) {
		ret = -ENOMEM;
		return ret;
	}

	/*4th: 设置事件分类及编码*/
//	mtdev->evbit[BIT_WORD(EV_ABS)] |= BIT_MASK(EV_ABS);
//	mtdev->evbit[BIT_WORD(EV_KEY)] |= BIT_MASK(EV_KEY);
	set_bit(EV_ABS, mtdev->evbit);
	set_bit(EV_KEY, mtdev->evbit);

//	mtdev->absbit[BIT_WORD(ABS_MT_POSITION_X)] |= BIT_MASK(ABS_MT_POSITION_X);
//	mtdev->absbit[BIT_WORD(ABS_MT_POSITION_Y)] |= BIT_MASK(ABS_MT_POSITION_Y);
//	mtdev->absbit[BIT_WORD(ABS_MT_PRESSURE)]   |= BIT_MASK(ABS_MT_PRESSURE);	
	set_bit(ABS_MT_POSITION_X, mtdev->absbit);
	set_bit(ABS_MT_POSITION_Y, mtdev->absbit);
	set_bit(ABS_MT_PRESSURE, mtdev->absbit);

//	mtdev->keybit[BIT_WORD(BTN_TOUCH)]	= BIT_MASK(BTN_TOUCH);
	set_bit(BTN_TOUCH,  mtdev->keybit);

	/*5th: 注册输入设备对应的字符设备驱动*/
	ret = input_register_device(mtdev);
	if (ret) {
		goto error0;
	}

	/*初始化中断下半部任务对象*/
	tasklet_init(&task, do_4key_bh, 0);

	ret = register_4keys();
	if (ret < 0) {
		goto error1;
	}

	/*初始化内核定时器*/
	setup_timer(&timer, do_timer, 0);

	return 0;
error1:
	input_unregister_device(mtdev);
	return ret;
error0:
	input_free_device(mtdev);
	return ret;
}
module_init(demo_init);

static void __exit demo_exit(void)
{
	tasklet_kill(&task);
	unregister_4keys();
	del_timer_sync(&timer);
	input_unregister_device(mtdev);
}
module_exit(demo_exit);

MODULE_LICENSE("GPL");
