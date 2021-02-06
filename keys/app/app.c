#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define CNT 4 

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/keys\n", str);
}

//./app /dev/keys
int main(int argc, char *argv[])
{
	int fd;
	char buf[CNT] = { 0 };
	char oldbuf[CNT] = { 0 };
	int i;

	int ret;

	if (argc != 2) {
		usage(argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(1);
	}

	while (1) {
		ret = read(fd, buf, CNT); //应该实现进程的阻塞
		if (ret != CNT) {
			perror("read");
			exit(1);
		}
		
		for (i = 0; i < CNT; i++) {
			if (oldbuf[i] != buf[i]) {
				printf("key%d is %s.\n", i+1, buf[i]?"down":"up");
				oldbuf[i] = buf[i];
			}
		}
	}

	return 0;
}
