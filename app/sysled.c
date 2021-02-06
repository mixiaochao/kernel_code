#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define CNT 4 

enum {
	LEDON = 1,
	LEDOFF,
	LEDSTAT
};

ssize_t sys_ledctl(int cmd, int arg)
{
	ssize_t ret;

	__asm__ __volatile__ (
		
		"mov r0, %[cmd]\n"
		"mov r1, %[arg]\n"

		"svc #(0x900000+378)\n"

		"mov %[ret], r0\n"

		:[ret]"=&r"(ret)
		:[arg]"r"(arg),[cmd]"r"(cmd)
		:"r0","r1"
	);

	return ret;
}

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s on/off num\n", str);
	fprintf(stderr, "      %s state\n", str);
	exit(1);
}

//./a.out on/off which
//./a.out state
int main(int argc, char *argv[])
{
	int fd;
	int ret;
	char buf[CNT];
	int num;
	int i;

	if ((argc != 3) && (argc != 2)) {
		usage(argv[0]);
	}


	if (argc == 2) {
		if (!strncmp("state", argv[1], 5)) {
			ret = sys_ledctl(LEDSTAT, (int)buf);		
			assert(ret == 0);
			for (i = 0; i < 4; i++) {
				printf("led_%d is %s.\n", i+1, buf[i]==LEDON?"on":"off");	
			}
		} else {
			usage(argv[0]);
		}

		return 0;
	}

	num = atoi(argv[2]);

	if (!strncmp("on", argv[1], 2)) {
		ret = sys_ledctl(LEDON, num);	
	} else if (!strncmp("off", argv[1], 3)) {
		ret = sys_ledctl(LEDOFF, num);	
	} else {
		usage(argv[0]);
	}

	assert(ret == 0);

	return 0;
}
