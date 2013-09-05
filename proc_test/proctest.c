#include <linux/proc_fs.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/jiffies.h>


struct timer_list pt_timer;
wait_queue_head_t pt_wait;
static int condition = 0;

void pt_timeout(unsigned long data)
{
	condition = 1;
	wake_up(&pt_wait);
}

static int proc_test_read(char *buf, char **start, off_t offset, int len, int *unused_i, void *unused_v)
{
	init_timer(&pt_timer);
	init_waitqueue_head(&pt_wait);

	pt_timer.function = pt_timeout;
	pt_timer.data = 0;
	pt_timer.expires = jiffies + HZ;

	init_timer(&pt_timer);
	add_timer(&pt_timer);
	printk("%s, HZ: %d .\n",__func__,HZ);
	printk("%s, before jiffies: %d .\n",__func__,jiffies);
	wait_event_interruptible_timeout(pt_wait,condition,msecs_to_jiffies(50000));
	condition = 0;
	printk("%s, after jiffies: %d .\n",__func__,jiffies);
	return 0;
}

static __exit void proctest_exit(void)
{
	printk("%s, Enter",__func__);
}

static __init int proctest_init(void)
{
	struct proc_dir_entry *proc_test;
	printk("%s, Enter",__func__);
	proc_test = create_proc_entry("proc_test",S_IRUGO,NULL);
		proc_test->read_proc = proc_test_read;
	return proc_test != NULL;
	
}
module_init(proctest_init);
module_exit(proctest_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Proc test");
