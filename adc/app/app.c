#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fddev;
	int adc_read_result = 0;
	int read_return;

	fddev = open("/dev/resistance", O_RDONLY);
	if (fddev < 0) {
		perror("open");
		exit(1);
	}

	while (1) {
		read_return = read(fddev, &adc_read_result, sizeof(int));
		if (read_return != sizeof(int)) {
			fprintf(stderr, "read error...\n");
			exit(1);
		}

		printf("%4d : %.2fmv\n", adc_read_result, (1800.0*adc_read_result)/4095);
		sleep(2);
	}	

	close(fddev);

	return 0;
}
