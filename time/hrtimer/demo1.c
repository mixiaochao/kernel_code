#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <linux/hrtimer.h>

#define SEC 6
#define NS  500

static struct hrtimer hrtimer;

static enum hrtimer_restart do_hrtimer_fool(struct hrtimer *hrtimer)
{
	printk("hrtimer time up to ...\n");

	hrtimer_forward_now(hrtimer, ktime_set(SEC, NS));

	return HRTIMER_RESTART;
}

static int demo_init(void)
{
	/*设置并初始化内核定时器*/
	hrtimer_init(&hrtimer,  CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer.function = do_hrtimer_fool;

	hrtimer_start(&hrtimer, ktime_set(SEC, NS), HRTIMER_MODE_REL);


	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
	hrtimer_cancel(&hrtimer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
