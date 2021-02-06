#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>

#include "kbufioctl.h"

#define CNT 512 

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/kbufx rd\n", str);
	fprintf(stderr, "      %s /dev/kbufx wr file\n", str);
}


//./ioctl /dev/kbufx rd
//./ioctl /dev/kbufx wr file 
int main(int argc, char *argv[])
{
	int fddev;
	int retr;
	int ret, fd;

	struct buf_data bufdata;

	if (argc != 3 && argc != 4) {
		usage(argv[0]);
		exit(1);
	}

	bufdata.buf = malloc(CNT);
	bufdata.len = malloc(sizeof(size_t));

	fddev = open(argv[1], O_RDWR| O_NDELAY);
	if (fddev < 0) {
		perror("open");
		exit(1);
	}

	if (argc == 3) {
		if (strcmp(argv[2], "rd") != 0) {
			usage(argv[0]);
			exit(1);
		}	 

		*bufdata.len = CNT - 1;
		ret = ioctl(fddev, USR_RD_BUF, (unsigned long)&bufdata);
		if (ret < 0) {
			perror("ioctl");
			exit(1);
		}

		bufdata.buf[*bufdata.len] = '\0';
		printf("%s\n", bufdata.buf);

		exit(0);
	}

	if (strcmp(argv[2], "wr") != 0) {
		usage(argv[0]);
		exit(1);
	}

	fd = open(argv[3], O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	ret = read(fd, bufdata.buf, CNT);
	if (ret < 0) {
		perror("read");
		exit(1);
	} else if (ret > 0) {
		*bufdata.len = ret;
	}

	printf("user ready write %luBytes into kernel...\n", *bufdata.len);
	ret = ioctl(fddev, USR_WR_BUF, &bufdata);
	if (ret < 0) {
		perror("ioctl");
		exit(1);
	}

	printf("write %lu bytes into device.\n", *bufdata.len);

	return 0;
}
