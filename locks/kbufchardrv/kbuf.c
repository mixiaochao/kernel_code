#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/delay.h>

/*
 	知识点：
		一套驱动如何支持多个设备。

		注册字符设备驱动的方式
			注册申请设备号
			初始化注册字符设备驱动

		非阻塞型IO

		自动创建设备文件
 */

/*设备的数量*/
#define KBUFNUM 16


#define DEVNAME "kbuf"

static struct kbuf {
	char *kbuf;
	size_t validlen;
	struct cdev cdev;
}kbufs[KBUFNUM];

static dev_t devnum;
static int major = 0;
static struct class *class;
static struct device *device;


static int kbuf_open (struct inode *inodp, struct file *filp)
{
	/*
	 	1. 可以拿到用户打开的设备文件的主次设备号
		2. inodp->i_cdev --> struct cdev *i_cdev = &kbufs[x].cdev;
	 */

	//1. filp->private_data = &kbufs[iminor(inodp)];
	
	//2. 
	filp->private_data = container_of(inodp->i_cdev, struct kbuf, cdev);

	return 0;
}

static ssize_t kbuf_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	struct kbuf *pdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}

	if (!pdev->validlen) {
		return -EAGAIN;
	}

	cnt = min(cnt, pdev->validlen);

	if (copy_to_user(buf, pdev->kbuf, cnt)) {
		return -EIO;
	}

	memmove(pdev->kbuf, pdev->kbuf+cnt, pdev->validlen - cnt);

	mdelay(50);

	pdev->validlen -= cnt;

	return cnt;
}

static ssize_t kbuf_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	struct kbuf *pdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}

	if (pdev->validlen == SZ_512) {
		return -ENOSPC;
	}

	cnt = min(cnt, SZ_512 - pdev->validlen);

	if (copy_from_user(pdev->kbuf + pdev->validlen, buf, cnt)) {
		return -EIO;
	}

	mdelay(50);

	pdev->validlen += cnt;

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

static int __init kbuf_init(void)
{
	int i, ret;

	/*

	 	为每个设备，即数组元素的kbuf分配空间。

		
		初始化字符设备驱动对象并注册字符设备驱动
	 */

	/* 申请注册设备号*/
	if (!major) {
		ret = alloc_chrdev_region(&devnum, 0, KBUFNUM, DEVNAME);

		if (ret == 0) {
			major = MAJOR(devnum);
		} else {
			return ret;
		}
	} else {
		devnum = MKDEV(major, 0);
		ret = register_chrdev_region(devnum, KBUFNUM, DEVNAME);
		if (ret < 0) {
			return ret;
		}
	}

	class = class_create(THIS_MODULE, DEVNAME);
	if (IS_ERR(class)) {
		ret = PTR_ERR(class);
		goto error0;
	}


	for (i = 0; i < KBUFNUM; i++) {
		kbufs[i].kbuf = kzalloc(SZ_512, GFP_KERNEL);   //GFP_KERNEL : 申请空间有能阻塞， GFP_ATOMIC: 不会阻塞
		if (NULL == kbufs[i].kbuf) {
			ret = -ENOMEM;
			goto error1;
		}
		cdev_init(&kbufs[i].cdev, &fops);

		ret = cdev_add(&kbufs[i].cdev, devnum + i, 1);
		if (ret < 0) {
			kfree(kbufs[i].kbuf);
			goto error1;
		}

		device = device_create(class, NULL, devnum + i, NULL, DEVNAME"%d", i);
		if (IS_ERR(device)) {
			cdev_del(&kbufs[i].cdev);
			kfree(kbufs[i].kbuf);
			goto error1;	
		}
	}

	return 0;

error1:
	while (i--) {
		device_destroy(class, devnum+i);
		kfree(kbufs[i].kbuf);
		cdev_del(&kbufs[i].cdev);
	}
	
	class_destroy(class);
error0:
	unregister_chrdev_region(devnum, KBUFNUM);

	return ret;

}
module_init(kbuf_init);


static void __exit kbuf_exit(void)
{
	int i = KBUFNUM;

	while (i--) {
		device_destroy(class, devnum+i);
		kfree(kbufs[i].kbuf);
		cdev_del(&kbufs[i].cdev);
	}
	
	class_destroy(class);
	unregister_chrdev_region(devnum, KBUFNUM);
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
