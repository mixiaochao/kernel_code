#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/io.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/poll.h>

#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/proc_fs.h>

/*
 	知识点：
 		在实现阻塞的基础上实现IO复用的poll接口。

		上层的select, poll, epoll调用的是内核驱动里的同一个
		接口poll。

 */

/*设备的数量*/
#define KBUFNUM 16


#define DEVNAME "kbuf"

static struct kbuf {
	char *kbuf;
	size_t validlen;
	struct cdev cdev;

	wait_queue_head_t waitr;    //1st
	wait_queue_head_t waitw;

	struct proc_dir_entry *kbufproc;
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
	int ret;
	struct kbuf *pdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}

	/*没有数据可读时，则阻塞*/
	ret = wait_event_interruptible(pdev->waitr, pdev->validlen != 0);
	if (ret < 0) {
		return ret;
	}

	cnt = min(cnt, pdev->validlen);

	if (copy_to_user(buf, pdev->kbuf, cnt)) {
		return -EIO;
	}

	memmove(pdev->kbuf, pdev->kbuf+cnt, pdev->validlen - cnt);
	pdev->validlen -= cnt;
	wake_up_interruptible(&pdev->waitw);

	return cnt;
}

static ssize_t kbuf_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	ssize_t ret;
	struct kbuf *pdev = filp->private_data;

	if (cnt == 0) {
		return cnt;
	}

	/*3rd: 没有空间可写，则等待*/
	ret = wait_event_interruptible(pdev->waitw, pdev->validlen != SZ_512);
	if (ret < 0) {
		return ret;
	}

	cnt = min(cnt, SZ_512 - pdev->validlen);

	if (copy_from_user(pdev->kbuf + pdev->validlen, buf, cnt)) {
		return -EIO;
	}

	pdev->validlen += cnt;
	wake_up_interruptible(&pdev->waitr);

	return cnt;
}

static unsigned int kbuf_poll (struct file *filp, struct poll_table_struct *wait)
{
	struct kbuf *pdev = filp->private_data;	

	unsigned int mask = 0;

	/*
	 	当前文件实现了几种阻塞，调用几次poll_wait.
	 */

	poll_wait(filp, &pdev->waitr, wait);
	poll_wait(filp, &pdev->waitw, wait);

	/*
	 	根据文件的资源准备mask。
	 */

	if (pdev->validlen != 0) {
		mask |= POLLIN | POLLRDNORM;
	}

	if (pdev->validlen != SZ_512) {
		mask |= POLLOUT | POLLWRNORM;
	}

	return mask;
}

static int kbuf_read_proc(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
	int len = 0;
	struct kbuf *pdev = data;

	len += sprintf(page + len, "The kbuf has %d Bytes data.\n", pdev->validlen);
	len += sprintf(page + len, "The kbuf have %d Bytes space.\n", SZ_512 - pdev->validlen);

	return len;
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
	.poll		= kbuf_poll,
	.release	= kbuf_release,
};

static int __init kbuf_init(void)
{
	int i, ret;
	char kbufprocname[SZ_32];

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

		kbufs[i].validlen = 0;

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

		init_waitqueue_head(&kbufs[i].waitr);   //2end
		init_waitqueue_head(&kbufs[i].waitw);

		sprintf(kbufprocname, "%s%d", DEVNAME, i);

		kbufs[i].kbufproc = create_proc_read_entry(kbufprocname, 0444, NULL, kbuf_read_proc, &kbufs[i]);
		if (NULL == kbufs[i].kbufproc) {
			cdev_del(&kbufs[i].cdev);
			kfree(kbufs[i].kbuf);
			device_destroy(class, devnum+i);
			goto error1;
		}
	}

	return 0;

error1:
	while (i--) {
		sprintf(kbufprocname, "%s%d", DEVNAME, i);
		device_destroy(class, devnum+i);
		kfree(kbufs[i].kbuf);
		cdev_del(&kbufs[i].cdev);
		remove_proc_entry(kbufprocname, NULL);
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
	char kbufprocname[SZ_32];

	while (i--) {
		device_destroy(class, devnum+i);
		kfree(kbufs[i].kbuf);
		cdev_del(&kbufs[i].cdev);

		sprintf(kbufprocname, "%s%d", DEVNAME, i);
		remove_proc_entry(kbufprocname, NULL);
	}
	
	class_destroy(class);
	unregister_chrdev_region(devnum, KBUFNUM);
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
