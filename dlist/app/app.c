#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define CNT 512 

#define SZ_32 32

#define LIMIT_STU 50

struct stu {
	char name[SZ_32];
	size_t old;
};

void usage(const char *str)
{
	fprintf(stderr, "usage:\n");
	fprintf(stderr, "      %s /dev/kstu wr name old\n", str);
	fprintf(stderr, "      %s /dev/kstu rd\n", str);
}

//./app /dev/kbufx wr name 27
//./app /dev/kbufx rd
int main(int argc, char *argv[])
{
	int fddev;
	struct stu stu;
	int ret;

	if (argc != 3 && argc != 5) {
		usage(argv[0]);
		exit(1);
	}

	fddev = open(argv[1], O_RDWR | O_NDELAY);
	assert(fddev >= 0);

	memset(&stu, 0, sizeof(stu));

	if (argc == 3) {
		if (strcmp("rd", argv[2]) != 0) {
			usage(argv[0]);
			exit(1);
		}
		
		ret = read(fddev, &stu, sizeof(stu));	
		if (ret < 0) {
			perror("read");
			exit(1);
		} else if (ret == sizeof(stu)) {
			printf("stu %s %lu years old.\n", stu.name, stu.old);
		}
		
		return 0;
	}

	strncpy(stu.name, argv[3], SZ_32 - 1);
	stu.old = atoi(argv[4]);

	ret = write(fddev, &stu, sizeof(stu));
	if (ret != sizeof(stu)) {
		perror("write");
		exit(1);
	}

	return 0;
}
