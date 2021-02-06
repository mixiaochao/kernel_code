#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define CNT 512 

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/kbufx\n", str);
	exit(1);
}

//./rd /dev/kbufx
int main(int argc, char *argv[])
{
	int fddev;
	int retr;
	char buf[CNT];

	if (argc != 2) {
		usage(argv[0]);
	}

	fddev = open(argv[1], O_RDONLY | O_NDELAY);
	assert(fddev >= 0);

	retr = read(fddev, buf, CNT - 1);
	if (retr < 0) {
		perror("read");
		exit(1);
	}

	buf[retr] = '\0';

	printf("--- read %d Bytes ok! ---\n", retr);
	printf("%s\n", buf);

	return 0;
}
