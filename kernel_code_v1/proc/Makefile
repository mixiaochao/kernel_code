obj-m	:= proc.o

KERNEL	:= ~/linux-3.5/

all:
	make -C $(KERNEL) M=`pwd` modules;
	mv *.ko /rootfs/
clean:
	make -C $(KERNEL) M=`pwd` clean 
	
