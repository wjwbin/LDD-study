#include <stdio.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include <time.h>
#include "../driver/polltest.h"

pthread_t pid;
struct poll_thread_t
{
	pthread_mutex_t mutex;
	pthread_cond_t cond_v;
	int fd;
};

struct poll_thread_t data;

void* poll_thread1(void *arg)
{
	int times = 10;
	int ret = 0;
	struct pollfd poll_fd[1];

	pthread_mutex_lock(&data.mutex);
	pthread_cond_signal(&data.cond_v);
	pthread_mutex_unlock(&data.mutex);
	poll_fd[0].fd = data.fd;
	poll_fd[0].events = POLLIN;
	while(times--)
	{
		ret = poll(poll_fd,1,3000);
		if(ret > 0)
		{
			if(poll_fd[0].revents & POLLIN)
			{
				printf("thread1: !! poll wake up. times: %d\n",times);
				ret = ioctl(data.fd,POLLTEST_CLEAR,0);
				if(ret < 0)
				{
					printf("ioctl command clear failed.\n");
					return NULL;
				}
				sleep(2);
			}

		}
		else if(ret == 0)
		{
			printf("thread1: poll time out. times: %d\n",times);
		}
		else
			printf("poll failed.\n");
	}
	return NULL;
}

/*
void* poll_thread2(void *arg)
{
	int times = 10;
	int ret = 0;
	struct pollfd poll_fd[1];

	pthread_mutex_lock(&data.mutex);
	pthread_cond_signal(&data.cond_v);
	pthread_mutex_unlock(&data.mutex);
	poll_fd[0].fd = data.fd;
	poll_fd[0].events = POLLIN;
	while(times--)
	{
		ret = poll(poll_fd,1,3000);
		if(ret > 0)
		{
			if(poll_fd[0].revents & POLLIN)
			{
				printf("thread2:!! poll wake up. times: %d\n",times);
				sleep(1);
				ret = ioctl(data.fd,POLLTEST_CLEAR,0);
				if(ret < 0)
				{
					printf("ioctl command clear failed.\n");
					return NULL;
				}
			}

		}
		else if(ret == 0)
		{
			printf("thread2: poll time out. times: %d\n",times);
		}
		else
			printf("poll failed.\n");
	}
	return NULL;
}
*/

int main(void)
{
	int ret;
	int i = 0;

	pthread_mutex_init(&data.mutex,NULL);
	pthread_cond_init(&data.cond_v,NULL);
	data.fd = open("/dev/polltest_dev",O_RDWR);
	if(data.fd < 0)
	{
		printf("Open file error.\n");
		return -1;
	}

	//create the first thread.
	pthread_mutex_lock(&data.mutex);
	ret = pthread_create(&pid,NULL,poll_thread1,NULL);
	if(ret != 0)
	{
		printf("pthread create failed.\n");
		return -1;
	}
	else
		pthread_cond_wait(&data.cond_v,&data.mutex);
	pthread_mutex_unlock(&data.mutex);

/*
	//create the second thread.
	pthread_mutex_lock(&data.mutex);
	ret = pthread_create(&pid,NULL,poll_thread2,NULL);
	if(ret != 0)
	{
		printf("pthread create failed.\n");
		return -1;
	}
	else
		pthread_cond_wait(&data.cond_v,&data.mutex);
	pthread_mutex_unlock(&data.mutex);
*/
	sleep(1);
	for(i = 0; i < 10; i++)
	{
		ret = ioctl(data.fd,POLLTEST_WAKEUP,0);
		printf("send wake up command.\n");
		if(ret < 0)
		{
			printf("ioctl command wake up failed.\n");
			return -1;
		}
		sleep(1);
	}
	ret = pthread_join(pid,NULL);
	if(ret != 0)
	{
		printf("can't join the thread.\n");
		return -1;
	}
	return 0;
}
