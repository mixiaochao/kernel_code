#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/epoll.h>

#define CNT 512 

#define NUM 16

/*
	
	select, poll, 
	
	epoll

 	epoll_create
	epoll_ctl
	epoll_wait
 */

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/kbufx\n", str);
	exit(1);
}

//./rd
int main(int argc, char *argv[])
{
	int fd;
	int i;
	int ret, retep;
	char buf[CNT];
	int epfd;
	struct epoll_event epevent;
	struct epoll_event eparr[NUM];

	epfd = epoll_create(NUM);
	if (epfd < 0) {
		perror("epoll_create");
		exit(1);
	}

	for (i = 0; i < NUM; i++) {
		sprintf(buf, "/dev/kbuf%d", i);
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
		retep = epoll_wait(epfd, eparr, NUM, -1);
		if (retep < 0) {
			perror("epoll_wait");
			exit(1);
		} else if (retep == 0) {
			printf("TimeOut...\n");
			continue;
		} 

		printf("retep = %d\n", retep);

		for (i = 0; i < retep; i++) {
			if (eparr[i].events & EPOLLIN) {
				ret = read(eparr[i].data.fd, buf, CNT - 1);
				if (ret < 0) {
					perror("read");
					exit(1);
				}

				buf[ret] = '\0';

				printf("--- read %d Bytes ok! ---\n", ret);
				printf("%s\n", buf);
			}
		}
	}

	return 0;
}
