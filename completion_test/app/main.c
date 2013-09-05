#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <pthread.h>
#include "../driver/cptest.h"


pthread_t pid;
pthread_mutex_t mutex;
pthread_cond_t cond_v;
int fd;

void* test_thread(void *arg)
{
	int ret = 0;

	pthread_mutex_lock(&mutex);
	pthread_cond_signal(&cond_v);
	pthread_mutex_unlock(&mutex);

	sleep(1);
	ret = ioctl(fd,COMPLETE_CP);
	if(ret < 0)
		printf("error: ioctl wake up. 1st\n");
	else
		printf("success: ioctl wake up. 1st\n");
/*
	ret = ioctl(fd,COMPLETE_CP);
	if(ret < 0)
		printf("error: ioctl wake up.2nd\n");
	else
		printf("success: ioctl wake up.2nd\n");

*/	return NULL;
}

int main(void)
{
	int ret = 0;
	fd = open("/dev/cptest_device",O_RDWR);
	if(fd < 0)
	{
		printf("error: open file.\n");
		return -1;
	}
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&cond_v,NULL);
	
	pthread_mutex_lock(&mutex);
	ret = pthread_create(&pid,NULL,test_thread,NULL);
	if(ret != 0)
	{
		printf("error: create thread.\n");
		return -1;
	}
	pthread_cond_wait(&cond_v,&mutex);
	pthread_mutex_unlock(&mutex);

	ret = ioctl(fd,WAIT_FOR_CP);
	if(ret < 0)
		printf("error: ioctl wait.1st\n");
	else
		printf("success: ioctl wait.1st\n");

	sleep(2);

	ret = ioctl(fd,WAIT_FOR_CP);
	if(ret < 0)
		printf("error: ioctl wait.2nd\n");
	else
		printf("success: ioctl wait.2nd\n");

	return 0;

}
