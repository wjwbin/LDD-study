#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/mm.h>

#define DEVICE "test_dev"
#define KMALLOC_SIZE PAGE_SIZE*4096+1

struct cdev *test_cdev = NULL;
struct class *test_class;
dev_t dev;
unsigned char* buffer;


static int test_open(struct inode *inode,struct file *filp)
{
	buffer = (unsigned char* )kmalloc(KMALLOC_SIZE,GFP_KERNEL);
	if(buffer == NULL)
	{
		printk("test_open error!\n");
		return -1;
	}
	SetPageReserved(virt_to_page(buffer));
	return 0;
}

static int test_release(struct inode *inode,struct file *filp)
{
	if(buffer)
		kfree(buffer);
	return 0;
}

static int test_read(struct inode *inode,struct file *filp)
{
	return 0;
}

static int test_write(struct inode *inode,struct file *filp)
{
	int i = 0;
	for(i = 0;i < KMALLOC_SIZE; i++)
	{
		*((char*)buffer + i) = 0xFF & i;
	}
	return 0;
}

static int test_mmap(struct file *filp,struct vm_area_struct *vma)
{
	unsigned long pos,pfn;
	pos = virt_to_phys(buffer);
	pfn = pos >> PAGE_SHIFT;
	if(remap_pfn_range(vma,vma->vm_start,pfn,vma->vm_end-vma->vm_start,PAGE_SHARED))
		return -EAGAIN;
	return 0;
}

struct file_operations test_ops = 
{
	.open = test_open,
	.read = test_read,
	.write = test_write,
	.release = test_release,
	.mmap = test_mmap,
};

static int test_init(void)
{
	int ret = -1;
	ret = alloc_chrdev_region(&dev,0,1,DEVICE);
	if(ret < 0)
	{
		printk("error, can't alloc major number for the device");
		return ret;
	}
	
	test_cdev = cdev_alloc();
	test_cdev->ops = &test_ops;
	test_cdev->owner = THIS_MODULE;
	cdev_add(test_cdev,dev,1);

	test_class = class_create(THIS_MODULE,"test_class");
	if(IS_ERR(test_class))
	{
		printk("Error: failed in creating class\n");
		return -1;
	}

	device_create(test_class,NULL,dev,NULL,"test_dev");
	return 0;
}

static void test_exit(void)
{
	cdev_del(test_cdev);

	device_destroy(test_class,dev);
	class_destroy(test_class);

	unregister_chrdev_region(dev,1);
}

module_init(test_init);
module_exit(test_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("wenbinw");
