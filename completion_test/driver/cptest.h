#include <linux/ioctl.h>

#define CPTEST_IOC_MAGIC 'k'

#define WAIT_FOR_CP _IOR(CPTEST_IOC_MAGIC,1,int)
#define COMPLETE_CP _IOW(CPTEST_IOC_MAGIC,2,int)
