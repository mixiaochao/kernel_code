obj-m	:= spin.o

spin-objs := demo.o irq.o

KERNEL	:= ~/linux-3.5
#KERNEL	:= /lib/modules/`uname -r`/build/

INSTALL_MYPATH := ~/rootfs


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` INSTALL_MOD_PATH=$(INSTALL_MYPATH) modules_install 
	depmod -A
	
