#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>

#define SZ 512 

#define PROCESS_NR  200

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/spin\n", str);
}

//./app /dev/spin
int main(int argc, char *argv[])
{
	int fddev;
	int read_num;
	char buf[SZ];
	pid_t pid[PROCESS_NR] = { 0 };
	int ret = 0;
	int i;

	if (argc != 2) {
		usage(argv[0]);
		exit(1);
	}

	fddev = open(argv[1], O_RDWR | O_NDELAY);
	if (fddev < 0) {
		perror("open");
		exit(1);
	}

	for (i = 0; i < PROCESS_NR; i++) {
		pid[i] = fork();
		if (pid[i] < 0) {
			perror("fork");
			ret = -1;
			goto error0;
		} else if (pid[i] == 0) {
			errno = 0;

			snprintf(buf, SZ, "[process <%d>...]", getpid());

			ret = ioctl(fddev, 0, buf);
			if (ret < 0) {
				perror("ioctl");
			}

			close(fddev);

			return;
		} 
	}

error0:
	while (i--) {
		waitpid(pid[i], NULL, 0);
	}
	close(fddev);

	return ret;
}
