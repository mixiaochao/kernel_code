obj-m	:= demo0.o

#KERNEL	:= ~/linux-3.5
KERNEL	:= /lib/modules/`uname -r`/build/


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` modules_install 
	depmod -A
	
