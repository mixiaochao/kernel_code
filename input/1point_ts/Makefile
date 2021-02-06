obj-m	:= key.o

KERNEL	:= /home/mxc/linux-3.5/

INSTALL_MYPATH := ~/rootfs


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` INSTALL_MOD_PATH=$(INSTALL_MYPATH) modules_install 
	depmod -A
	
