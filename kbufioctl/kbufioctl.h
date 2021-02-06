#ifndef KBUF_IOCTL_H_
#define KBUF_IOCTL_H_

#include <linux/ioctl.h>

#define CNT 512

struct buf_data {
	char *buf;
	size_t *len;
};

#define USRBUF_TYPE 'm'

#define USR_RD_BUF _IOWR(USRBUF_TYPE, 0, int)
#define USR_WR_BUF _IOWR(USRBUF_TYPE, 1, int)


#endif
