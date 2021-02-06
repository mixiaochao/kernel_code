#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#include <linux/miscdevice.h>
#include <linux/gpio.h>

#define DEVNAME "miscled"


enum {
	LEDON = 0,
	LEDOFF,
	LEDSTAT
};


static int leds[4] = {
	EXYNOS4X12_GPM4(0),	
	EXYNOS4X12_GPM4(1),	
	EXYNOS4X12_GPM4(2),	
	EXYNOS4X12_GPM4(3),	
};

static void ledonoff(int cmd, int num)
{
	num--;
	gpio_set_value(leds[num], cmd);	
}

static int ledstat(char *stat)
{
	u8 tmp[4];	
	int i;

	for (i = 0; i < 4; i++) {
		tmp[i] = gpio_get_value(leds[i]);
	}

	return copy_to_user(stat, tmp, 4);
}

static int demo_open (struct inode *inodp, struct file *filp)
{
	printk("open %d, %d device is ok!\n", imajor(inodp), iminor(inodp));

	return 0;
}

static ssize_t demo_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	ssize_t ret;

	if (cnt != 4) {
		return -EINVAL;
	}

	if ((ret = ledstat(buf)) < 0) {
		return ret;	
	}

	return cnt;
}

static ssize_t demo_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	char kbuf[2];
	//考虑用户通过buf传递两个字符作为控制led的亮灭状态及编号

	if (cnt != 2) {
		return -EINVAL;
	}

	//从用户空间拷贝数据到内核空间，请用copy_from_user或这个get_user
	if (copy_from_user(kbuf, buf, cnt) != 0) {
		return -EIO;
	}

	if (kbuf[0] != LEDON && kbuf[0] != LEDOFF) {
		return -EINVAL;
	}

	if (kbuf[1] < 1 || kbuf[1] > 4) {
		return -EINVAL;
	}

	ledonoff(kbuf[0], kbuf[1]);

	return cnt;
}

static int demo_release (struct inode *inodp, struct file *filp)
{
	printk("close %d, %d device is ok!\n", imajor(inodp), iminor(inodp));

	return 0;
}

static struct file_operations fops = {
	.owner 	 = THIS_MODULE,
	.open	 = demo_open,
	.read	 = demo_read,
	.write	 = demo_write,
	.release = demo_release,
};

static struct miscdevice miscled = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int __init demo_init(void)
{
	int ret;
	int i;


	for (i = 0; i < ARRAY_SIZE(leds); i++) {
		ret = gpio_request(leds[i], DEVNAME);
		if (ret < 0) {
			goto error0;
		}

		ret = gpio_direction_output(leds[i], LEDOFF);
		if (ret < 0) {
			goto error0;
		}
	}


	return misc_register(&miscled);
error0:
	while (i--) {
		gpio_free(leds[i]);
	}

	return ret;
}


module_init(demo_init);


static void __exit demo_exit(void)
{
	int i;

	i = 4;
	while (i--) {
		gpio_set_value(leds[i], LEDOFF);
		gpio_free(leds[i]);
	}

	misc_deregister(&miscled);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
