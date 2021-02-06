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
	fprintf(stderr, "      %s /dev/kbufx file\n", str);
	exit(1);
}

//./wr /dev/kbufx file 
int main(int argc, char *argv[])
{
	int fddev, fdsrc;
	int retr, retw;
	char buf[CNT];

	if (argc != 3) {
		usage(argv[0]);
	}

	fddev = open(argv[1], O_WRONLY | O_NDELAY);
	if (fddev < 0) {
		perror("open");
		exit(1);
	}

	fdsrc = open(argv[2], O_RDONLY | O_NDELAY);
	if (fdsrc < 0) {
		perror("open");
		exit(1);
	}

	retr = read(fdsrc, buf, CNT);
	assert(retr > 0);

	retw = write(fddev, buf, retr);	
	if (retw < 0) {
		perror("write");
		exit(1);
	}

	printf("Write %d Bytes ok!\n", retw);

	return 0;
}
