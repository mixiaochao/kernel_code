#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include <errno.h>

#define LEN 512 

#define PROCESS_NR 200

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
	int read_num, write_num;
	char buf[LEN];
	int ret = 0;
	int i;
	pid_t pid[PROCESS_NR];

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


	for (i = 0; i < PROCESS_NR; i++) {
		pid[i] = fork();
		if (pid[i] < 0) {
			perror("fork");
			ret = -1;
			goto error0;
		} else if (pid[i] == 0) {
			errno = 0;
			lseek(fdsrc, 0, SEEK_SET);
			read_num = read(fdsrc, buf, LEN);
			if (read_num < 0) {
				fprintf(stderr, "[read process %d]: %s\n", getpid(), strerror(errno));
				close(fdsrc);
				close(fddev);
				return;
			}

			write_num = write(fddev, buf, read_num);	
			if (write_num < 0) {
				fprintf(stderr, "[write process %d]: %s\n", getpid(), strerror(errno));
			} else if (write_num > 0) {
				printf("process %d write %dbytes ok!\n", getpid(), write_num);	
			} else {
				printf("process %d write %dbytes ok!\n", getpid(), write_num);	
			}
			
			close(fdsrc);
			close(fddev);
			return;
		} 
	}

error0:
	while (i--) {
		waitpid(pid[i], NULL, 0);
	}
	close(fddev);
	close(fdsrc);

	return ret;
}
