#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>

#include <linux/atomic.h>

#define DEVNAME "spin"

/*
 	原子操作的情况是，当驱动程序中定义了整形的全局变量而导致多进程的访问存在竞争，那么替换成
	原子类型的变量，并通过调用原子变量的函数接口就可以解决竞争问题。

	但是必须清楚，只有当对整形变量进行加减操作的情况下才可以使用原子变量解决竞争问题。

	#define atomic_inc(v)		atomic_add(1, v)
	#define atomic_dec(v)		atomic_sub(1, v)
	
	#define atomic_inc_and_test(v)	(atomic_add_return(1, v) == 0)
	#define atomic_dec_and_test(v)	(atomic_sub_return(1, v) == 0)
	#define atomic_inc_return(v)    (atomic_add_return(1, v))
	#define atomic_dec_return(v)    (atomic_sub_return(1, v))
	#define atomic_sub_and_test(i, v) (atomic_sub_return(i, v) == 0)
	
	#define atomic_add_negative(i,v) (atomic_add_return(i, v) < 0)


	原子操作实现的本质是:

	ldrex

	strex
 */

//static int open_flags = 1;
static atomic_t open_flags = ATOMIC_INIT(1);

static int atomic_open (struct inode *inodp, struct file *filp)
{
#if 0
	open_flags--;

	if (open_flags < 0) {
		open_flags++;

		return -EBUSY;
	}
#endif
	if (!atomic_sub_and_test(1, &open_flags)) {
		atomic_inc(&open_flags);
		return -EBUSY;	
	}

	return 0;
}

static int atomic_release (struct inode *inodp, struct file *filp)
{
	if (atomic_read(&open_flags) == 0) {
		atomic_inc(&open_flags);
	}

	return 0;
}

static struct file_operations fops = {
	.owner 	 	= THIS_MODULE,
	.open		= atomic_open,
	.release   	= atomic_release,
};

static struct miscdevice misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int __init demo_init(void)
{
	return misc_register(&misc);
}

module_init(demo_init);

static void __exit demo_exit(void)
{
	misc_deregister(&misc);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
