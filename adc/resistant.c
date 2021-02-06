#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <plat/adc.h>

#define DEVNAME "resistance"

#define RESISTANCE_CHANNEL 0

static struct s3c_adc_client *client;

static ssize_t resis_read (struct file *filp, char __user *buf, size_t cnt, loff_t *fpos)
{
	int adc_read_result = 0;

	if (sizeof(int) != cnt) {
		return -EINVAL;
	}	

	adc_read_result = s3c_adc_read(client, RESISTANCE_CHANNEL);
	if (adc_read_result < 0) {
		return adc_read_result;
	}

	//put_user/get_user
	if (put_user(adc_read_result, (int *)buf) < 0) {
		return -EIO;
	}

	return sizeof(int);
}

static struct file_operations fops = {
	.owner	=  	THIS_MODULE,
	.read	=	resis_read,
};

static struct miscdevice misc = {
	.minor 	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int resistance_probe (struct platform_device *pdev)
{
	int ret;

	/*注册申请s3c_adc_client*/
	client = s3c_adc_register(pdev, NULL, NULL, 0);
	if (IS_ERR(client)) {
		return PTR_ERR(client);
	}

	/*注册混杂字符设备驱动*/
	ret = misc_register(&misc);
	if (ret < 0) {
		s3c_adc_release(client);
		return ret;
	}

	return 0;
}

static int resistance_remove (struct platform_device *pdev)
{
	s3c_adc_release(client);
	misc_deregister(&misc);

	return 0;
}

static struct platform_driver resistancedrv = {
	.probe	= resistance_probe,
	.remove	= resistance_remove,
	.driver	= {
		.name	=  "resistance",
	},
};

module_platform_driver(resistancedrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
