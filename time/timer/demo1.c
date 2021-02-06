#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <linux/timer.h>

#define SEC 3

static struct timer_list timer;

static void do_timer_fool(unsigned long data)
{
	printk("%s\n", (char *)data);

	mod_timer(&timer, jiffies + SEC*HZ);
}

static int demo_init(void)
{
	/*设置并初始化内核定时器*/
	setup_timer(&timer, do_timer_fool, (unsigned long)"hello");
	timer.expires = jiffies + SEC*HZ;

	/*注册启动定时器*/
	add_timer(&timer);

	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
	del_timer_sync(&timer);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
