#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define CNT 4 

//控制led的枚举常量
enum {
	LEDON = 0,
	LEDOFF,
	LEDSTAT
};

void usage(const char *str, const char *devstr)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s %s on/off num\n", str, devstr);
	fprintf(stderr, "      %s %s state\n", str, devstr);
	exit(1);
}

//./a.out /dev/led on/off which
//./a.out /dev/led state
int main(int argc, char *argv[])
{
	int fd;
	int ret;
	char buf[CNT];
	char bufcmd[2] = {0, 0};
	int num;
	int i;

	if ((argc != 4) && (argc != 3)) {
		usage(argv[0], argv[1]);
	}

	fd = open(argv[1], O_RDWR | O_NDELAY);
	assert(fd >= 0);

	if (argc == 3) {
		if (!strncmp("state", argv[2], 5)) {
			ret = read(fd, buf, CNT);
			assert(ret == 4);
			for (i = 0; i < 4; i++) {
				printf("led_%d is %s.\n", i+1, buf[i]==LEDON?"on":"off");	
			}
		} else {
			usage(argv[0], argv[1]);
		}

		return 0;
	}

	bufcmd[1] = atoi(argv[3]);

	if (!strncmp("on", argv[2], 2)) {
		bufcmd[0] = LEDON;	
	} else if (!strncmp("off", argv[2], 3)) {
		bufcmd[0] = LEDOFF;
	} else {
		usage(argv[0], argv[1]);
	}

	ret = write(fd, bufcmd, 2);
	assert(ret == 2);

	close(fd);

	return 0;
}
