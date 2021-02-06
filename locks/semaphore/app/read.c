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
	fprintf(stderr, "      %s /dev/kbufx\n", str);
	exit(1);
}

//./rd /dev/kbufx
int main(int argc, char *argv[])
{
	int fddev;
	int read_num;
	char buf[SZ+1];
	pid_t pid[PROCESS_NR] = { 0 };
	int ret = 0;
	int i;

	if (argc != 2) {
		usage(argv[0]);
	}

	fddev = open(argv[1], O_RDONLY | O_NDELAY);
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
			read_num = read(fddev, buf, SZ);
			if (read_num < 0) {
				fprintf(stderr, "[process%d read]: %s\n",  
							getpid(), strerror(errno));
				close(fddev);
				return;
			} else if (read_num > 0) {
				buf[read_num] = '\0';
				printf("------ process[%d] read %d Bytes ok! ------\n", 
							getpid(), read_num);
				printf("%s\n", buf);

				close(fddev);
				return;
			} else {
				printf("------ process[%d] read %d Bytes ok! ------\n", 
							getpid(), read_num);
				close(fddev);
				return;
			}
		} 
	}

error0:
	while (i--) {
		waitpid(pid[i], NULL, 0);
	}
	close(fddev);

	return ret;
}
