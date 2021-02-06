obj-m	:= mybus.o mydev.o mydrv.o 

KERNEL	:= ~/linux-3.5
INSTALL_MYPATH := ~/rootfs


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` INSTALL_MOD_PATH=$(INSTALL_MYPATH) modules_install 
	depmod -A
	
