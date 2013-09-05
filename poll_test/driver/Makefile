obj-m := polltest.o
KERNELDIR := /usr/src/linux-2.6.38
CC = gcc
default:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean
