obj-m := cptest.o
KERNEL_DIR := /usr/src/linux-2.6.38
CC = gcc
default:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) modules

clean:
	$(MAKE) -C $(KERNEL_DIR) M=$(shell pwd) clean
