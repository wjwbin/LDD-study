#include <linux/ioctl.h>

#define POLLTEST_WAKEUP _IOW('T',1,int)
#define POLLTEST_CLEAR _IOW('T',2,int)
