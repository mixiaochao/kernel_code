#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <linux/wait.h>


/*
 	time_before/time_before_eq
	time_after/time_after_eq
 */

void mysleep(int sec)
{
	wait_queue_head_t wait;

	init_waitqueue_head(&wait);

	wait_event_interruptible_timeout(wait, 0, sec*HZ);
}

void mydelay(int sec)
{
	unsigned long end = jiffies + sec*HZ;

	while (time_before(jiffies, end)) {
		schedule(); //将当前进程的时间片让出去
	}
}

void up_time(void)
{
	unsigned long sec  = jiffies/HZ;
	unsigned long hour = sec/3600;
	unsigned long min  = sec%3600/60;
	sec = sec%60;

	printk("%lu-%lu:%lu\n", hour, min, sec);
}

static int demo_init(void)
{
	printk("HZ = %d, jiffies = %lu!\n", HZ, jiffies);

	up_time();
	mysleep(3);
	up_time();

	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
	printk("goold, world!\n");
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
