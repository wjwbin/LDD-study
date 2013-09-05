#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/completion.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/device.h>
#include <linux/slab.h>
#include "cptest.h"

#define DEVICE "cptest_device"

dev_t dev;
struct class *class;

struct cptest_device
{
	struct cdev *cdev;
	struct completion cp_completion;
};

struct cptest_device *cp_dev = NULL;

static int cptest_open(struct inode *inode,struct file *filp)
{
	printk("%s,Enter\n",__func__);
	return 0;
}

static int cptest_release(struct inode *inode, struct file *filp)
{
	printk("%s,Enter\n",__func__);
	return 0;
}

static long cptest_ioctl(struct file *filp,unsigned int cmd,unsigned long arg)
{
	printk("%s,Enter\n",__func__);
	switch(cmd){
	case WAIT_FOR_CP:
		printk("%s, wait_for_completion.\n",__func__);
		wait_for_completion_interruptible(&cp_dev->cp_completion);
		break;
	case COMPLETE_CP:
		printk("%s, wake_up_completion.\n",__func__);
		complete(&cp_dev->cp_completion);
		break;
	default:
		pr_err("%s, illegal cmd.\n",__func__);
		return -ENOIOCTLCMD;
	}
	return 0;
}

struct file_operations fops = {
	.open = cptest_open,
	.unlocked_ioctl = cptest_ioctl,
	.release = cptest_release,

};
static int __init cptest_init(void)
{
	int ret = 0;
	struct device *cp_device;
	printk("%s,Enter\n",__func__);
	ret = alloc_chrdev_region(&dev,0,1,"cptest");
	if(ret < 0)
	{
		pr_err("%s,Error at line %d\n",__func__,__LINE__);
		return ret;
	}
	cp_dev = kmalloc(sizeof(struct cptest_device),GFP_KERNEL);
	if(!cp_dev)
	{
		pr_err("%s,Error at line %d\n",__func__,__LINE__);
		return -1;
	}
	init_completion(&cp_dev->cp_completion);
	cp_dev->cdev = cdev_alloc();
	cp_dev->cdev->ops = &fops;
	cp_dev->cdev->owner = THIS_MODULE;
	cdev_add(cp_dev->cdev,dev,1);

	class = class_create(THIS_MODULE,"cptest_class");
	if(IS_ERR(class))
	{
		pr_err("%s,Error at line %d\n",__func__,__LINE__);
		return -1;
	}
	cp_device = device_create(class,NULL,dev,NULL,DEVICE);
	if(IS_ERR(cp_device))
	{
		pr_err("%s,Error at line %d\n",__func__,__LINE__);
		return -1;
	}
	return 0;
}

static void __exit cptest_exit(void)
{
	cdev_del(cp_dev->cdev);
	device_destroy(class,dev);
	class_destroy(class);
	unregister_chrdev_region(dev,1);
	kfree(cp_dev);
	printk("%s,Exit\n",__func__);
}


module_init(cptest_init);
module_exit(cptest_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("completion test");
