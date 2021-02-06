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

/*tasklet机制: 一个中断的下半部任务对应如下一个变量*/
struct tasklet_struct task;

extern void print_string(const char *str);

/*中断处理的下半部*/
void do_bh_key(unsigned long data)
{
	char kbuf[SZ_64] = "";
	struct key_t *pdev = (void *)data;
	pdev->cnt++;
	snprintf(kbuf, SZ_64, "%s is %s!\n", pdev->irqname, pdev->cnt%2?"down":"up");

	print_string(kbuf);
}

void free_irq_key(void)
{
	int i;

	for (i = 0; i < 4; i++) {
		free_irq(keys[i].irq, &keys[i]);
	}
}


/*注册中断时注册的中断处理函数为中断处理的上半部*/
static irqreturn_t do_key_irq_tp(int irq, void *dev)
{
	task.data = (unsigned long)dev;
	tasklet_schedule(&task);

	return IRQ_HANDLED;
}

int request_irq_key(void)
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

