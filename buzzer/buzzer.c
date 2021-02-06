#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/pwm.h>
#include <linux/device.h>
#include <linux/miscdevice.h>
#include <linux/gpio.h>
#include <linux/fs.h>
#include <plat/gpio-cfg.h>

#include "buzzerioctl.h"

#define DEVNAME "buzzer"

#define PWM_ID 0

#define NSPERSEC 1000000000UL

static struct buzzerdev_t {
	int gpionum;
	struct pwm_device *pwm;
}buzzer;

static void set_pwm_hz(unsigned long hz)
{
	int period_ns = NSPERSEC/hz;

	pwm_config(buzzer.pwm, period_ns >> 1, period_ns);
	s3c_gpio_cfgpin(buzzer.gpionum, S3C_GPIO_SFN(0x2));
	pwm_enable(buzzer.pwm);
}

static void stop_buzzer(void)
{
	pwm_disable(buzzer.pwm);
	gpio_direction_output(buzzer.gpionum, 0);
}

static long buzzer_ioctl (struct file *filp, unsigned int request, unsigned long arg)
{
	switch (request) {
		case SET_BUZZER_HZ:
			if (arg > 4000) {
				return -EINVAL;
			}
			set_pwm_hz(arg);
			break;
		case STOP_BUZZER:
			stop_buzzer();
			break;
		default:
			break;
	}

	return 0;
}

static struct file_operations fops = {
	.owner	=  THIS_MODULE,
	.unlocked_ioctl = buzzer_ioctl,
};

static struct miscdevice misc = {
	.minor 	= MISC_DYNAMIC_MINOR,
	.name	= DEVNAME,
	.fops	= &fops,
};

static int buzzer_probe (struct platform_device *pdev)
{
	int ret;

	struct resource *res = platform_get_resource(pdev, IORESOURCE_MEM, 0);

	/*buzzer: 全局变量，封装了访问设备时需要的gpio编号和pwm设备*/
	buzzer.gpionum = res->start;

	/*申请资源: gpio*/
	ret = gpio_request(res->start, "buzzer");
	if (ret < 0) {
		return -EBUSY;
	}
	gpio_direction_output(buzzer.gpionum, 0);

	/*申请资源: 申请id为0的pwm控制器对应的资源*/
	buzzer.pwm = pwm_request(PWM_ID, "buzzer");
	if (IS_ERR(buzzer.pwm)) {
		gpio_free(res->start);
		return PTR_ERR(buzzer.pwm);
	}


	/*注册字符设备驱动: 注册混杂字符设备驱动*/
	ret = misc_register(&misc);
	if (ret < 0) {
		pwm_free(buzzer.pwm);
		gpio_free(res->start);
		return ret;
	}
		
	return 0;
}

static int buzzer_remove (struct platform_device *pdev)
{
	stop_buzzer();
	pwm_free(buzzer.pwm);
	gpio_free(buzzer.gpionum);
	misc_deregister(&misc);

	return 0;
}

static struct platform_driver buzzerdrv = {
	.probe	= buzzer_probe,
	.remove	= buzzer_remove,
	.driver	= {
		.name	=  "buzzer",
	},
};

module_platform_driver(buzzerdrv);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Zhuangzhuang");
MODULE_VERSION("3.0");
MODULE_DESCRIPTION("It is a simple example for driver module.");
