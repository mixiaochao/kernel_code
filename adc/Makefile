obj-m	:= resistant.o 

KERNEL	:= ~/linux-3.5/


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` modules_install 
	depmod -A
	
