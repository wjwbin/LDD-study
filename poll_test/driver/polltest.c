#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/mm.h>
#include <linux/wait.h>
#include <linux/poll.h>

#include "polltest.h"


#define DEVICE "polltest_dev"
struct class *polltest_class;

struct cdev *polltest_cdev = NULL;
dev_t dev;
wait_queue_head_t polltest_queue;
int flag = 0;

static int polltest_open(struct inode *inode,struct file *file)
{
	flag = 0;
	printk("polltest_open enter.\n");
	return 0;
}

static int polltest_release(struct inode *inode,struct file *file)
{
	printk("polltest_release enter.\n");
	return 0;
}

static int polltest_read(struct inode *inode,struct file *file)
{
	printk("polltest_read enter.\n");
	return 0;
}

static int polltest_write(struct inode *inode,struct file *file)
{
	printk("polltest_write enter.\n");
	return 0;
}

static unsigned int polltest_poll(struct file *file,struct poll_table_struct *wait)
{
	unsigned int mask = 0;
	poll_wait(file,&polltest_queue,wait);
	printk("%s: flag: %d.\n",__func__,flag);
	if(flag)
		mask |= POLLIN;
	return mask;
}

static int polltest_ioctl(struct inode *inode,unsigned int cmd,unsigned long arg)
{
	int i = 0;
	int ret = 0;
	switch(cmd)
	{
		case POLLTEST_WAKEUP:
			printk("polltest_ioctl enter.\n");
			flag++;
			printk("wakeup: flag: %d.\n",flag);
			wake_up(&polltest_queue);
			break;
		case POLLTEST_CLEAR:
			flag--;
			printk("clear: flag: %d.\n",flag);
			break;
		default:
			printk("invalid parameter.\n");
			break;
	}
	return 0;
}

struct file_operations polltest_ops =
{
	.open = polltest_open,
	.read = polltest_read,
	.write = polltest_write,
	.release = polltest_release,
	.poll = polltest_poll,
	.unlocked_ioctl = polltest_ioctl,
};

static int polltest_init(void)
{
	int ret = -1;
	ret = alloc_chrdev_region(&dev,0,1,DEVICE);
	if(ret < 0)
	{
		printk("error, can't alloc major number for the device");
		return ret;
	}
	printk("poll polltest init enter.\n");

	polltest_cdev = cdev_alloc();
	polltest_cdev->ops = &polltest_ops;
	polltest_cdev->owner = THIS_MODULE;
	cdev_add(polltest_cdev,dev,1);

	polltest_class = class_create(THIS_MODULE,"polltest_class");
	if(IS_ERR(polltest_class))
	{
		printk("class polltest create failed.\n");
		return -1;
	}
	ret = device_create(polltest_class,NULL,dev,NULL,"polltest_dev");

	init_waitqueue_head(&polltest_queue);
	return 0;
}

static void polltest_exit(void)
{
	cdev_del(polltest_cdev);
	device_destroy(polltest_class,dev);
	class_destroy(polltest_class);
	unregister_chrdev_region(dev,1);
}

module_init(polltest_init);
module_exit(polltest_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenbinw");
