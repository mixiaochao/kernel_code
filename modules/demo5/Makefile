obj-m	:= kernelpri.o
#kernelpri-objs := demo0.o demo1.o
kernelpri-y := demo0.o demo1.o

KERNEL	:= /lib/modules/`uname -r`/build/


all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
install:
	make -C $(KERNEL) M=`pwd` modules_install 
	depmod -A
	
