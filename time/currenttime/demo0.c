#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>

#include <linux/delay.h>
#include <linux/time.h>

#define SEC 3

/*
   	1. 
	struct timespec current_kernel_time(void);

	struct timespec {
		__kernel_time_t	tv_sec;
		long		tv_nsec;
	};

	2. 
	void do_gettimeofday(struct timeval *tv);
	struct timeval {
		__kernel_time_t		tv_sec;	
		__kernel_suseconds_t	tv_usec;
	};
 */

#define NSPERSEC 1000000000ULL
#define USPERSEC 1000000UL

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
	struct timespec ts_start, ts_end;
	unsigned long long nsec = 0;

	struct timeval tv_start, tv_end;
	unsigned long usec = 0;


	ts_start = current_kernel_time();  //12223s, 10ns
	mdelay(13);
	ts_end = current_kernel_time(); //12227s, 2ns

	nsec = (ts_end.tv_sec - ts_start.tv_sec)*NSPERSEC + ts_end.tv_nsec - ts_start.tv_nsec;

	printk("current_kernel_time --> mdelay() = %lluns\n", nsec);

	do_gettimeofday(&tv_start);
	mdelay(13);
	do_gettimeofday(&tv_end);
	usec = (tv_end.tv_sec - tv_start.tv_sec)*USPERSEC + tv_end.tv_usec - tv_start.tv_usec;
	printk("do_gettimeofday --> mdelay()     = %luus\n", usec);


	return 0;
}

module_init(demo_init);


static void demo_exit(void)
{
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
