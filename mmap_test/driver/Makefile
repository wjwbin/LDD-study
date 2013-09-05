obj-m := test.o
KERNELDIR := /home/wenbinw/share/image/linux-2.6.38
CC = gcc
default:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) modules
clean:
	$(MAKE) -C $(KERNELDIR) M=$(shell pwd) clean
