obj-m	:= demo0.o demo1.o

KERNEL	:= ~/linux-3.5

all:
	make -C $(KERNEL) M=`pwd` modules
clean:
	make -C $(KERNEL) M=`pwd` clean 
