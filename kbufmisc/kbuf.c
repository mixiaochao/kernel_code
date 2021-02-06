#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/sizes.h>
#include <linux/device.h>
#include <linux/slab.h>

#include <linux/miscdevice.h>

#define DEVNAME "kbufmisc"

static struct kbuf {
	char *kbuf;
	size_t validlen;
	struct cdev cdev;
}kbufs;

static int kbuf_open (struct inode *inodp, struct file *filp)
{
	return 0;
}

static ssize_t kbuf_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt == 0) {
		return cnt;
	}

	if (!kbufs.validlen) {
		return -EAGAIN;
	}

	cnt = min(cnt, kbufs.validlen);

	if (copy_to_user(buf, kbufs.kbuf, cnt)) {
		return -EIO;
	}

	memmove(kbufs.kbuf, kbufs.kbuf+cnt, kbufs.validlen - cnt);
	kbufs.validlen -= cnt;

	return cnt;
}

static ssize_t kbuf_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	if (cnt == 0) {
		return cnt;
	}

	if (kbufs.validlen == SZ_512) {
		return -ENOSPC;
	}

	cnt = min(cnt, SZ_512 - kbufs.validlen);

	if (copy_from_user(kbufs.kbuf + kbufs.validlen, buf, cnt)) {
		return -EIO;
	}

	kbufs.validlen += cnt;

	return cnt;
}


static int kbuf_release (struct inode *inodp, struct file *filp)
{
	return 0;
}

static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.open		= kbuf_open,
	.read		= kbuf_read,
	.write		= kbuf_write,
	.release	= kbuf_release,
};

static struct miscdevice misc = {
	.minor 	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int __init kbuf_init(void)
{
	int ret;

	kbufs.kbuf = kzalloc(SZ_512, GFP_KERNEL);   //GFP_KERNEL : 申请空间有能阻塞， GFP_ATOMIC: 不会阻塞
	if (NULL == kbufs.kbuf) {
		return -ENOMEM;	
	}

	ret = misc_register(&misc);
	if (ret < 0) {
		kfree(kbufs.kbuf);
		return ret;
	}

	return 0;

}
module_init(kbuf_init);


static void __exit kbuf_exit(void)
{
	kfree(kbufs.kbuf);
	misc_deregister(&misc);	
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
