#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "usr_rdwr_proc"

#define USER_LIMITS SZ_1M

static struct prochead {
	struct list_head head;
	unsigned long total;
}prochead;

struct userdata {
	char *kbuf;
	struct list_head list;
};

static struct proc_dir_entry *procdir = NULL;

static int usr_proc_read(char *page, char **start, off_t off,
			  int count, int *eof, void *data)
{
	int len = 0;
	struct userdata *pos;

	len += sprintf(page + len, "------------------------ total %lu Bytes -------------------------\n\n",
						prochead.total);

	list_for_each_entry(pos, &prochead.head, list) {
		len += sprintf(page + len, "%s", pos->kbuf);
	}

	len += sprintf(page + len, "\n");

	return len;
}

static int usr_proc_write(struct file *file, const char __user *buffer,
			   unsigned long count, void *data)
{
	int ret;

	struct userdata *node;

	if (count == 0) {
		return 0;
	}	

	if (prochead.total >= USER_LIMITS) {
		return -ENOMEM;
	}	

	count = min(count, USER_LIMITS - prochead.total);

	node = kzalloc(sizeof(struct userdata), GFP_KERNEL);
	if (NULL == node) {
		return -ENOMEM;
	}

	node->kbuf = kzalloc(count+1, GFP_KERNEL);
	if (NULL == node->kbuf) {
		kfree(node);
		return -ENOMEM;
	}

	if ((ret = copy_from_user(node->kbuf, buffer, count))) {
		return ret;
	}

	node->kbuf[count] = '\0';

	list_add_tail(&node->list, &prochead.head);

	prochead.total += count;

	return count;
}

static int __init demo_init(void)
{
	prochead.total = 0;
	INIT_LIST_HEAD(&prochead.head);	

	procdir = create_proc_entry(PROC_NAME, 0644, NULL);
	if (NULL == procdir) {
		return -ENOMEM;
	}

	procdir->read_proc = usr_proc_read;
	procdir->write_proc = usr_proc_write;

	return 0;
}

module_init(demo_init);


static void __exit demo_exit(void)
{
	struct userdata *pos, *next;

	remove_proc_entry(PROC_NAME, NULL);

	list_for_each_entry_safe(pos, next, &prochead.head, list) {
		kfree(pos->kbuf);	
		kfree(pos);
	}
}

module_exit(demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
