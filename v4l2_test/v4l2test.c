#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/wait.h>
#include <linux/poll.h>
#include <linux/videodev2.h>
#include <media/v4l2-dev.h>

static int v4l2test_init(void)
{

	return 0;
}

static void v4l2test_exit(void)
{
	return;
}

module_init(v4l2test_init);
module_exit(v4l2test_exit);
MODULE_DESCRIPTION("v4l2 driver test");
MODULE_LICENSE("GPL v2");
