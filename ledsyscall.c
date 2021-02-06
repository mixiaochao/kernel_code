#include <linux/init.h>
#include <linux/module.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>

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

SYSCALL_DEFINE2(ledctl, int, cmd, int, arg)
{
	if ((cmd != LEDON) && (cmd != LEDOFF) && (cmd != LEDSTAT)) {
		return -EINVAL;
	}		

	if (cmd != LEDSTAT) {
		if (arg < 1 || arg > 4) {
			return -EINVAL;
		}

		ledonoff(cmd, arg);

		return 0;
	}

	return ledstat((char *)arg);	
}

static int led_init(void)
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

	return 0;
}

module_init(led_init);

MODULE_LICENSE("GPL");
