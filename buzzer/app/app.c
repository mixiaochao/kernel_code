#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "buzzerioctl.h"

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s sethz hz\n", str);
	fprintf(stderr, "      %s stop\n", str);
}

// ./app sethz hz
// ./app stop
int main(int argc, char *argv[])
{
	int fddev;
	int hz;
	int ret;

	if ((argc != 3) && (argc != 2)) {
		usage(argv[0]);
		exit(1);
	}

	fddev = open("/dev/buzzer", O_WRONLY | O_NDELAY);
	if (fddev < 0) {
		perror("open");
		exit(1);
	}

	if (argc == 3) {
		if (strcmp("sethz", argv[1]) != 0) {
			usage(argv[0]);
			close(fddev);
			exit(1);
		}	

		hz = atoi(argv[2]);
		if ((hz < 0) || (hz > 4000)) {
			fprintf(stderr, "hz %d is out of the range of buzzer hz.\n",
					hz);
			close(fddev);
			exit(1);
		}

		ret = ioctl(fddev, SET_BUZZER_HZ, hz);
		if (ret < 0) {
			perror("ioctl");
			close(fddev);
			exit(1);
		}

		return 0;
	}

	if (strcmp("stop", argv[1]) == 0) {
		ret = ioctl(fddev, STOP_BUZZER);
		if (ret < 0) {
			perror("ioctl");
			close(fddev);
			exit(1);
		}	
	}

	close(fddev);

	return 0;
}
