#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/sched.h>

#define DEVNAME "led"

static int major = 0;


#define GPM4BASE 0x11000000
#define GPM4CON	 0x2e0
#define GPM4DAT  0x2e4

typedef struct {
	int lednum;
	int ledcmd;
}led_ctl_t;

/*自己封装的对应设备的内核类*/
static struct tiny4412_led_t {
	char *reg; //
	struct cdev cdev; //代表一个字符设备驱动对象
}leds;

struct proc_dir_entry *proc = NULL;

static int proc_read (char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
	u32 tmp;
	int i;
	int ret = 0;

	tmp = ioread32(leds.reg + GPM4DAT);
	for (i = 0; i < 4; i++) {
		if (tmp & (1 << i)) {
			ret += sprintf(page+ret, "led %d is off.\n", i+1);
		} else {
			ret += sprintf(page+ret, "led %d is on.\n", i+1);
		}
	}

	return ret;
}

static int led_ctr(int lednum, int ledcmd)
{
	u32 tmp;

	tmp = ioread32(leds.reg + GPM4DAT);

	if ((lednum < 1) || (lednum > 4)) {
		return -EINVAL;
	}

	if (ledcmd == 1) {
		tmp &= ~(1 << (lednum - 1));
	} else if (ledcmd == 0) {
		tmp |= (1 << (lednum - 1));
	} else {
		return -EINVAL;
	}

	iowrite32(tmp, leds.reg + GPM4DAT);

	return 0;
}

static ssize_t  demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	led_ctl_t val;
	int ret;

	if (cnt != sizeof(led_ctl_t)) {
		return -EINVAL;
	}

	if (copy_from_user(&val, buf, sizeof(led_ctl_t))) {
		return -EINVAL;
	}

	
	ret = led_ctr(val.lednum, val.ledcmd);
	if (ret < 0) {
		return ret;
	}

	return cnt;
}

/*代表了字符设备对应的一套驱动集*/
static struct file_operations fops = {
	.owner		=  THIS_MODULE,
	.write		=  demo_write,
};

static struct class *class = NULL;
static struct device *device = NULL;
static dev_t devnum; //设备号类型的变量，保存设备号

static int __init demo_init(void)
{
	int ret;
	u32 tmp;

	/*1st：动态申请设备号*/
	ret = alloc_chrdev_region(&devnum, 0, 1, DEVNAME);
	major = MAJOR(devnum);

	if (ret < 0) {
		return ret;
	}

	class = class_create(THIS_MODULE, DEVNAME);
	if (IS_ERR(class)) {
		ret = PTR_ERR(class); 
		goto error0;
	}

	/*2nd: 初始化字符设备对象对应的struct cdev对象*/
	cdev_init(&leds.cdev, &fops);

	/*3rd：向内核注册字符设备驱动*/
	ret = cdev_add(&leds.cdev, devnum, 1);
	if (ret < 0) {
		goto error1;
	}
	
		/*在文件系统的/dev目录下自动创建设备文件*/
	device = device_create(class, NULL, devnum, NULL, DEVNAME);
	if (IS_ERR(device)) {
		ret = PTR_ERR(device); 
		goto error2;
	}

	/*将GPM4BASE物理地址映射成内核的虚拟地址*/
	leds.reg = ioremap(GPM4BASE, SZ_4K);
	if (NULL == leds.reg) {
		goto error3;
	}

	/*设置GPM4[0:3]引脚为输出功能*/
	tmp = ioread32(leds.reg + GPM4CON);
	tmp &= ~0xffff;
	tmp |= 0x1111;
	iowrite32(tmp, leds.reg + GPM4CON);

	/*默认让4个LED灭*/
	tmp = ioread32(leds.reg + GPM4DAT);
	tmp |= 0xf;
	iowrite32(tmp, leds.reg + GPM4DAT);

	proc = create_proc_entry("leds", 0444, NULL);
	if (NULL == proc) {
		goto error4;
	}
	proc->read_proc = proc_read;

	return 0;
error4:
	iounmap(leds.reg);
error3:
	device_destroy(class, devnum);
error2:
	cdev_del(&leds.cdev);
error1:
	class_destroy(class);
error0:
	unregister_chrdev_region(devnum, 1);

	return ret;
}


/*指定驱动模块的入口函数的宏*/
module_init(demo_init);


static void __exit demo_exit(void)
{
	int i;

	for (i = 0; i < 4; i++) {
		led_ctr(i+1, 0);
	}

	iounmap(leds.reg);

	device_destroy(class, devnum);
	class_destroy(class);
	unregister_chrdev_region(devnum, 1);
	cdev_del(&leds.cdev);

	remove_proc_entry("leds", NULL);

	printk("unregister %s is ok!\n", DEVNAME);
}

module_exit(demo_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("laomi9527");
MODULE_VERSION("mi plus 108");
MODULE_DESCRIPTION("example for driver module");
