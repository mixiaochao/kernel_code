#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>
#include <linux/input.h>

/* 输入设备的设备文件路径及名称  /dev/input/eventx */

#define CNT 3

#define SZ 32

/*
	
	select, poll, 
	
	epoll

 	epoll_create
	epoll_ctl
	epoll_wait
 */

static void input_device_read(int fd)
{
	struct input_event ev;
	int ret = 0;

	ret = read(fd, &ev, sizeof(ev));
	assert(ret == sizeof(ev));

	switch (ev.type) {
		case EV_SYN:
			if (ev.code == SYN_REPORT) {
				printf("..... syn report .....\n");
			} else if (ev.code == SYN_MT_REPORT) {
				printf("... syn mt report ...\n");
			} else {
				printf("... syn other report ...\n");
			}
			break;
		case EV_KEY:
			printf("key %dcode is %s.\n", ev.code, ev.value?"down":"up");
			break;
		case EV_REL:
			if (ev.code == REL_X) {
				printf("x = %d\n", ev.value);
			} else if (ev.code == REL_Y) {
				printf("y = %d\n", ev.value);
			} else {
				printf("rel: %d,%d\n", ev.code, ev.value);
			}
			break;
		case EV_ABS:
			if ((ev.code == ABS_X) || (ev.code == ABS_MT_POSITION_X)) {
				printf("x = %d\n", ev.value);
			} else if ((ev.code == ABS_Y) || (ev.code == ABS_MT_POSITION_Y)) {
				printf("y = %d\n", ev.value);
			} else if ((ev.code == ABS_PRESSURE) || (ev.code == ABS_MT_PRESSURE)) {
				printf("%s touchscreen.\n", ev.value?"down":"up");
			} else {
				printf("abs: %d, %d\n", ev.code, ev.value);
			}
			break;
		default:
			break;
	}
}

int main(int argc, char *argv[])
{
	int fd;
	int i;
	int ret, retep;
	char buf[SZ];
	int epfd;
	struct epoll_event epevent;
	struct epoll_event eparr[CNT];

	epfd = epoll_create(CNT);
	if (epfd < 0) {
		perror("epoll_create");
		exit(1);
	}

	for (i = 0; i < CNT; i++) {
		sprintf(buf, "/dev/input/event%d", i);
		fd = open(buf, O_RDONLY);
		if (fd < 0) {
			perror("open");
			exit(1);
		}

		epevent.events = EPOLLIN;
		epevent.data.fd = fd;
		ret = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &epevent);
		if (ret < 0) {
			perror("epoll_ctl");
			exit(1);
		}
	}

	while (1) {
		retep = epoll_wait(epfd, eparr, CNT, -1);
		if (retep < 0) {
			perror("epoll_wait");
			exit(1);
		} else if (retep == 0) {
			printf("TimeOut...\n");
			continue;
		} 

		for (i = 0; i < retep; i++) {
			if (eparr[i].events & EPOLLIN) {
				input_device_read(eparr[i].data.fd);
			}
		}
	}

	return 0;
}
