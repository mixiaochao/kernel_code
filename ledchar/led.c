#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/kernel.h>

#define DEVNAME "demo"

static int major = 100;

#define GPM4BASE 0x11000000
#define GPM4CON  0x2e0
#define GPM4DAT  0x2e4

enum {
	LEDON = 1,
	LEDOFF,
	LEDSTAT
};

u8 *gpm4base = NULL;

static void ledonoff(int cmd, int num)
{
	u8 tmp;

	tmp = ioread8(gpm4base + GPM4DAT);

	if (cmd == LEDON) {
		tmp &= ~(1 << (num - 1));
	} else {
		tmp |= (1 << (num - 1));
	}

	iowrite8(tmp, gpm4base + GPM4DAT);
}

static int ledstat(char *stat)
{
	u8 tmp[4];	
	u8 s;
	int i;

	s = ioread8(gpm4base + GPM4DAT); 

	for (i = 0; i < 4; i++) {
		tmp[i] = s&(1 << i)? LEDOFF : LEDON;
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

	printk("[kernel]: led%d is %s\n", kbuf[1], kbuf[0]==1?"on":"off");

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

static int __init demo_init(void)
{
	u32 tmp;

	gpm4base = ioremap(GPM4BASE, SZ_4K);	
	if (NULL == gpm4base) {
		return -ENOMEM;
	}

	tmp = ioread32(gpm4base + GPM4CON);
	tmp &= ~0xffff;
	tmp |= 0x1111;
	iowrite32(tmp, gpm4base + GPM4CON);

	tmp = ioread8(gpm4base + GPM4DAT);
	tmp |= 0xf;
	iowrite8(tmp, gpm4base + GPM4DAT);

	return register_chrdev(major, DEVNAME, &fops);
}

module_init(demo_init);


static void __exit demo_exit(void)
{
	u8 tmp;

	tmp = ioread8(gpm4base + GPM4DAT);
	tmp |= 0xf;
	iowrite8(tmp, gpm4base + GPM4DAT);

	iounmap(gpm4base);

	unregister_chrdev(major, DEVNAME);
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
