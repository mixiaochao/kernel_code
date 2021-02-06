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

#include <linux/types.h>
#include <linux/list.h>

#define DEVNAME "kstu"

#define LIMIT_STU 50

struct stu {
	char name[SZ_32];
	size_t old;
};

struct stu_node {
	struct stu stu;
	struct list_head list;
};

struct stu_head_t {
	struct list_head head;
	size_t num;
}stuhead = {
	.num = 0
};

static ssize_t kbuf_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	struct stu_node *stu;

	if (cnt != sizeof(struct stu)) {
		return -EINVAL;
	}

	if (stuhead.num == 0) {
		return 0;
	}

	stu = container_of(stuhead.head.next, struct stu_node, list);

	if (copy_to_user(buf, &stu->stu, cnt)) {
		return -EINVAL;
	}	

	list_del(stuhead.head.next);
	stuhead.num--;

	kfree(stu);

	return cnt;
}

static ssize_t kbuf_write (struct file *filp, const char __user *buf, size_t cnt, loff_t *fpos)
{
	struct stu_node *n;
	int ret;

	if (cnt != sizeof(struct stu)) {
		return -EINVAL;
	}

	if (stuhead.num > LIMIT_STU) {
		return -EAGAIN;
	}

	n = kmalloc(sizeof(struct stu_node), GFP_KERNEL);
	if (NULL == n) {
		return -ENOMEM;
	}

	if ((ret = copy_from_user(&n->stu, buf, cnt))) {
		return ret;		
	}

	list_add_tail(&n->list, &stuhead.head);
	stuhead.num++;

	return cnt;
}


static struct file_operations fops = {
	.owner 		= THIS_MODULE,
	.read		= kbuf_read,
	.write		= kbuf_write,
};

static struct miscdevice misc = {
	.minor 	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int __init kbuf_init(void)
{
	int ret;

	INIT_LIST_HEAD(&stuhead.head); /*让头节点的两个成员指针指向自己*/

	ret = misc_register(&misc);
	if (ret < 0) {
		return ret;
	}

	return 0;

}
module_init(kbuf_init);


static void __exit kbuf_exit(void)
{
	struct list_head *n, *pos;
	struct stu_node *pstu;

	list_for_each_safe(pos, n, &stuhead.head){
		pstu = container_of(pos, struct stu_node, list);
//		printk("%s %lu years old, say goodbye!\n", pstu->stu.name, pstu->stu.old);
		//printk(KERN_EMERG"%s %lu years old, say goodbye!\n", pstu->stu.name, pstu->stu.old);
		//printk("<0>""%s %lu years old, say goodbye!\n", pstu->stu.name, pstu->stu.old);
		printk("<0>%s %lu years old, say goodbye!\n", pstu->stu.name, pstu->stu.old);
		kfree(pstu);
	}

	misc_deregister(&misc);	
}
module_exit(kbuf_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("zhuangzhuang");
MODULE_VERSION("plus 3.0");
