#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#define CNT 64

ssize_t sys_read(int fd, void *buf, size_t count)
{
	ssize_t ret;

	__asm__ __volatile__ (
		
		"mov r0, %[fd]\n"
		"mov r1, %[buf]\n"
		"mov r2, %[count]\n"

		"svc #(0x900000+3)\n"

		"mov %[ret], r0\n"

		:[ret]"=&r"(ret)
		:[fd]"r"(fd),[buf]"r"(buf),[count]"r"(count)
		:"r0","r1","r2"
	);

	return ret;
}

//sys_fork

//sys_write

//./a.out  srcfile  dstfile
int main(int argc, char *argv[])
{
	int fd;
	int ret;
	char buf[CNT];

	if (argc != 2) {
		fprintf(stderr, "Usage: %s file.\n", argv[0]);
		exit(1);
	}

	fd = open(argv[1], O_RDONLY | O_NDELAY);
	assert(fd >= 0);

	while ((ret = sys_read(fd, buf, CNT)) > 0) {
		//sys_write(1, buf, ret);
		write(1, buf, ret);
	}

	assert(ret == 0);

	return 0;
}
