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

/*四个按键连接到exynos4412的GPX3[2..5], EINT26, EINT27, EINT28, EINT29*/
static struct key_t {
	int irq;
	char irqname[SZ_16];
	int cnt;
}keys[4] = {
	{ IRQ_EINT(26),	"key1", 0},
	{ IRQ_EINT(27),	"key2", 0},
	{ IRQ_EINT(28),	"key3", 0},
	{ IRQ_EINT(29),	"key4", 0},
};

/*工作队列机制: 一个中断的下半部任务对应如下一个变量*/
/*static struct work_struct work;*/

static struct keywork {
	struct work_struct work;
	struct key_t *pkey;
}keybh;

/*中断处理的下半部*/
static void do_bh_key(struct work_struct *work)
{
	struct key_t *pdev = container_of(work, struct keywork, work)->pkey; 
	pdev->cnt++;
	printk("%s is %s!\n", pdev->irqname, pdev->cnt%2?"down":"up");

	if (in_interrupt()) {
		printk("%s is excuted in interrupt...\n", __func__);
	} else {
		printk("%s is excuted in process...\n", __func__);
	}
}

static void free_irq_key(void)
{
	int i;

	for (i = 0; i < 4; i++) {
		free_irq(keys[i].irq, &keys[i]);
	}
}


/*注册中断时注册的中断处理函数为中断处理的上半部*/
static irqreturn_t do_key_irq_tp(int irq, void *dev)
{
	keybh.pkey = dev;
	schedule_work(&keybh.work); /*将下半部任务交给调度器在合适时间调度*/

	printk("In %s ...\n", __func__);

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

static int __init kbuf_init(void)
{
	/*初始化下版本任务对象*/
	INIT_WORK(&keybh.work, do_bh_key);

	return request_irq_key();

}
module_init(kbuf_init);


static void __exit kbuf_exit(void)
{
	free_irq_key();

	flush_work(&keybh.work);
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
