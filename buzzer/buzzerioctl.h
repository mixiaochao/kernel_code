#ifndef BUZZER_IOCTL_H_
#define BUZZER_IOCTL_H_

#include <linux/ioctl.h>

#define BUZZER_TYPE 'B'

#define SET_BUZZER_HZ  _IOW(BUZZER_TYPE, 0, int)
#define STOP_BUZZER    _IO(BUZZER_TYPE, 1)

#endif
