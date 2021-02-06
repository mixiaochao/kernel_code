#ifndef MYDEV_H_
#define MYDEV_H_

struct mydevice {
	struct device dev;  //驱动模型中用于和总线关联的设备结构体成员
	const char *name;
};

#endif
