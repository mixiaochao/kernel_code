obj-m	:= mybus.o
obj-m	+= mydev1.o 
obj-m	+= mydev2.o 
obj-m	+= mydev3.o 
obj-m	+= mydrv1.o 
obj-m	+= mydrv2.o 


KERNEL	:= ~/linux-3.5
INSTALL_MYPATH := ~/rootfs


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` INSTALL_MOD_PATH=$(INSTALL_MYPATH) modules_install 
	depmod -A
	
